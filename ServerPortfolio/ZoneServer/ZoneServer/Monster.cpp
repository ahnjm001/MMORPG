#include "Monster.h"

#include "MainThread.h"
#include "User.h"
#include "Field.h"
#include "SectorManager.h"
#include "Sector.h"

#include "PathFinderAgent.h"

Monster::Monster(Field* _field, FieldTilesData* _fieldTilesData, 
	SectorManager* _sectorManager, MonsterInfo& _info, MonsterData& _data)
{
	m_failed = false;

	m_info = _info;
	m_data = _data;
	m_info.hp = m_data.hp;

	m_spawnPosition.x = m_info.position.x;
	m_spawnPosition.y = m_info.position.y;

	m_isDeath = false;
	m_isAttack = false;

	m_currentTime = m_maxTime = 0.0f;

	TRYCATCH_CONSTRUCTOR(m_sendBuffer = new SendBuffer(2000), m_failed);
	if (m_failed) return;

	m_field = _field;
	m_fieldTilesData = _fieldTilesData;
	m_sectorManager = _sectorManager;
	m_sector = nullptr;

	InitializeCriticalSection(&m_cs);

	Spawn();
};

Monster::~Monster()
{
	if(m_sendBuffer != nullptr) delete m_sendBuffer;

	DeleteCriticalSection(&m_cs);

	m_tileList.clear();
	m_tileList.resize(0);
}

void Monster::Reset()
{
	m_sendBuffer->Reset();
}

void Monster::Update()
{
	m_currentTime += (1.0f / 10.0f);

	FSM();

	if (m_isAttack)
	{
		if (m_info.state != STATE::ATTACK)
		{
			m_info.state = STATE::ATTACK;
			m_isAttack = false;
		}
	}

	if (m_isDeath)
	{
		if (m_info.state != STATE::DEATH)
		{
			m_info.state = STATE::DEATH;
			m_currentTime = 0.0f;
		}
	}

	//printf("%d : position : %f, %f \n", m_info.index, m_info.position.x, m_info.position.y);
}

void Monster::Spawn()
{
	m_target = nullptr;

	m_info.hp.currentValue = m_info.hp.maxValue;

	m_info.position = m_spawnPosition;
	m_homeTile = m_fieldTilesData->GetTile(
		static_cast<int>(m_info.position.x),
		static_cast<int>(m_info.position.y));
	m_nowTile = m_homeTile;

	UpdateSector();

	m_info.state = STATE::SPAWN;

	m_isDeath = false;
	m_isAttack = false;

	MonsterInfoPacket* monsterInfoPacket = 
		reinterpret_cast<MonsterInfoPacket*>(m_sendBuffer->
			GetBuffer(sizeof(MonsterInfoPacket)));
	monsterInfoPacket->info = m_info;
	monsterInfoPacket->Init(SendCommand::Zone2C_MONSTER_INFO, sizeof(MonsterInfoPacket));

	m_sendBuffer->Write(monsterInfoPacket->size);

	//MainThread::getSingleton()->AddToMonsterPacketQueue({ this, monsterInfoPacket });
	m_field->SectorSendAll(m_sector->GetRoundSectorsVec(),
		reinterpret_cast<char*>(monsterInfoPacket), monsterInfoPacket->size);
}

void Monster::Move()
{
	VECTOR2 tempVec2 = VECTOR2(m_targetPosition - m_info.position);
	float magnitude = tempVec2.Magnitude();

	if (magnitude <= m_data.moveSpeed * (1.0f / 10.0f) || magnitude == 0.0f)
		m_info.position = m_targetPosition;

	//�� ����, ������ ����(���̰�,ȸ����) ���ϱ�
	//https://darkpgmr.tistory.com/121
	//VECTOR2(m_targetPosition - m_info.position) / magnitude �� �κ�

	//https://www.youtube.com/watch?v=WxWJorOVIj8
	//https://www.youtube.com/watch?v=-gokuNjpyNg

	m_info.position = m_info.position + tempVec2 / magnitude * m_data.moveSpeed * (1.0f / 10.0f);
	//���� * �̵��ӵ� * �ð�
}

void Monster::Attack()
{
	float magnitude = VECTOR2(m_info.position - m_target->GetInfo()->unitInfo.position).Magnitude();

	//�Ӵϱ� ����
	if (magnitude > (m_data.attackDistance * 2) - m_data.attackDistance)
	{
		m_targetTile = m_target->GetTile();

		if (m_targetTile == m_nowTile) return;

		//MYDEBUG("���� \n");

		PathFindStart(m_targetTile, STATE::MOVE);
		//m_info.state = STATE::MOVE;

		return;
	}

	if (m_target->GetInfo()->unitInfo.fieldNum != m_field->GetFieldNum() 
		|| m_target->IsDeath())
	{
		//MYDEBUG("�׾����� ���ư��� \n");

		m_targetTile = m_homeTile;
		PathFindStart(m_targetTile, STATE::RETURN);
		//m_info.state = STATE::RETURN;

		return;
	}

	if (m_currentTime >= m_data.attackDelay)
	{
		MonsterAttackPacket* monsterAttackPacket =
			reinterpret_cast<MonsterAttackPacket*>(m_sendBuffer->
				GetBuffer(sizeof(MonsterAttackPacket)));

		monsterAttackPacket->monsterIndex = m_info.index;
		monsterAttackPacket->damage = m_data.attackDamage;
		monsterAttackPacket->Init(SendCommand::Zone2C_MONSTER_ATTACK, sizeof(MonsterAttackPacket));

		m_sendBuffer->Write(monsterAttackPacket->size);
		//MainThread::getSingleton()->AddToMonsterPacketQueue({ this, monsterAttackPacket });
		/*m_field->SectorSendAll(m_sector->GetRoundSectorsVec(),
			reinterpret_cast<char*>(monsterAttackPacket), monsterAttackPacket->size);*/

		m_target->Hit(m_info.index, m_data.attackDamage);

		m_currentTime = 0.0f;
	}
}

Packet* Monster::Hit(User* _user, int _damage)
{
	if (m_info.state == STATE::DEATH) return nullptr;

	//��κ� �������� �̷��� ������ �ȵ���
	if (m_info.state == STATE::SPAWN || m_info.state == STATE::RETURN)
	{
		MonsterHitPacket* monsterHitPacket = 
			reinterpret_cast<MonsterHitPacket*>(m_sendBuffer->
			GetBuffer(sizeof(MonsterHitPacket)));
		monsterHitPacket->monsterIndex = m_info.index;
		monsterHitPacket->userIndex = _user->GetInfo()->userInfo.userID;
		monsterHitPacket->damage = 0;
		monsterHitPacket->Init(SendCommand::Zone2C_MONSTER_HIT_FAIL, sizeof(MonsterHitPacket));

		m_sendBuffer->Write(monsterHitPacket->size);

		return monsterHitPacket;
	}

	if (m_target == nullptr)
	{
		m_isAttack = true;
		m_target = _user;
	}

	m_info.hp.currentValue -= _damage;

	MonsterHitPacket* monsterHitPacket =
		reinterpret_cast<MonsterHitPacket*>(m_sendBuffer->
			GetBuffer(sizeof(MonsterHitPacket)));

	monsterHitPacket->monsterIndex = m_info.index;
	monsterHitPacket->userIndex = _user->GetInfo()->userInfo.userID;
	monsterHitPacket->damage = _damage;
	monsterHitPacket->Init(SendCommand::Zone2C_MONSTER_HIT, sizeof(MonsterHitPacket));

	m_sendBuffer->Write(monsterHitPacket->size);

	if (m_info.hp.currentValue <= 0)
	{
		m_isDeath = true;
	}

	return monsterHitPacket;
}

void Monster::FSM()
{
	switch (m_info.state)
	{
		//�⺻ ����
		case STATE::IDLE:
		{
			if (m_currentTime < m_data.patrolDelay) break;

			m_targetTile = m_nowTile;

			while (m_targetTile == m_nowTile)
			{
				int randX = (rand() % (m_data.patrolRange * 2)) - m_data.patrolRange;
				int randY = (rand() % (m_data.patrolRange * 2)) - m_data.patrolRange;

				m_targetTile = m_fieldTilesData->
					GetTile(m_homeTile->GetX() + randX, m_homeTile->GetY() + randY);
			}

			PathFindStart(m_targetTile, STATE::PATROL);
			break;
		}
		case STATE::READY:
		{
			if (m_currentTime <= 30.0f) break;

			MYDEBUG("[ %d monster : ReSend ] \n", m_info.index);
			m_targetTile = m_homeTile;
			PathFindStart(m_targetTile, STATE::RETURN);

			break;
		}
		//����
		case STATE::PATROL:
		{
			Move();
			//�̵����̶�� ��� PATROL ����������
			if (PathMove()) break;
			//�̵��� �������� IDLE��
			m_info.state = STATE::IDLE;
			m_currentTime = 0.0f;
			m_targetTile = nullptr;
			break;
		}
		//�̵�(���ݽ� �� ��쿡 ����)
		case STATE::MOVE:
		{
			Move();

			float magnitude_1 = VECTOR2(m_info.position - m_target->GetInfo()->unitInfo.position).Magnitude();

			//�����Ÿ� ���� ����
			if (magnitude_1 <= (m_data.attackDistance * 2) - m_data.attackDistance)
			{
				m_info.state = STATE::ATTACK;
				break;
			}

			float magnitude_2 = VECTOR2(m_info.position - m_spawnPosition).Magnitude();

			//��ȯ�Ÿ����� �־������ ��ȯ
			if (magnitude_2 > (m_data.returnDistance * 2) - m_data.returnDistance)
			{
				//MYDEBUG("�־ ��ȯ \n");

				m_targetTile = m_homeTile;
				PathFindStart(m_targetTile, STATE::RETURN);
				//m_info.state = STATE::RETURN;

				break;
			}

			//Ÿ�� ������ �������� Ÿ��Ÿ���� �ٲ���� ��� ����
			if (m_targetTile != m_target->GetTile())
			{
				m_targetTile = m_target->GetTile();
				PathFindStart(m_targetTile, STATE::MOVE);

				break;
			}

			//�̵����� �ƴϴ� == �����ߴ�. -> ����
			if (!PathMove())
			{
				m_info.state = STATE::ATTACK;
			}

			break;
		}
		//���� Ÿ�Ϸ� ���ư���
		case STATE::RETURN:
		{
			Move();

			//�̵� ����
			if (!PathMove())
			{
				//ü�� �ٽ� �����·�
				Spawn();

				m_currentTime = 0.0f;
			}
			break;
		}
		//����
		case STATE::ATTACK:
		{
			Attack();
			break;
		}
		//��� - 5�� �� Spawn
		case STATE::DEATH:
		{
			if (m_currentTime >= 5.0f)
			{
				Spawn();

				m_currentTime = 0.0f;
			}

			break;
		}
		//���� - ù ���� & ��Ȱ �� �� �̿��
		case STATE::SPAWN:
		{
			if (m_currentTime >= 1.0f)
			{
				m_currentTime = 0.0f;
				m_info.state = STATE::IDLE;
			}
			break;
		}
	}
}

void Monster::PathFindStart(Tile* _targetTile, STATE _state)
{
	//MYDEBUG("[ %d monster : Start ] \n", m_info.index);

	char sendBuffer[1000];

	m_info.state = STATE::READY;
	m_currentTime = 0;

	PathFindPacket* pathFindPacket = reinterpret_cast<PathFindPacket*>(sendBuffer);
		//reinterpret_cast<PathFindPacket*>(m_sendBuffer->GetBuffer(sizeof(PathFindPacket)));
	pathFindPacket->Init(SendCommand::Zone2Path_PATHFIND, sizeof(PathFindPacket));

	pathFindPacket->monsterNum = m_info.index;
	pathFindPacket->fieldNum = m_field->GetFieldNum();
	pathFindPacket->state = _state;

	pathFindPacket->nowPosition.x = (float)m_nowTile->GetX();
	pathFindPacket->nowPosition.y = (float)m_nowTile->GetY();
	pathFindPacket->targetPosition.x = (float)_targetTile->GetX();
	pathFindPacket->targetPosition.y = (float)_targetTile->GetY();

	//m_sendBuffer->Write(pathFindPacket->size);

	PathFinderAgent::getSingleton()->
		Send(reinterpret_cast<char*>(pathFindPacket), pathFindPacket->size);

	/*PathFinderAgent::getSingleton()->SendPathFindPacket(m_info.index, m_field->GetFieldNum(), _state,
		m_nowTile->GetX(), m_nowTile->GetY(), _targetTile->GetX(), _targetTile->GetY());*/

	/*PathFinding::PathFind(&m_tileList, m_nowTile, _targetTile);

	MYDEBUG("target : %d , %d \n", _targetTile->GetX(), _targetTile->GetY());

	if (m_tileList.size() == 0) return;

	m_targetPosition = m_tileList.front();

	m_tileList.pop_front();
	m_currentTime = 0;*/
}

void Monster::PathFindSuccess(PathFindPacket_Success* _packet)
{
	//MYDEBUG("[ %d monster : Success ] \n", m_info.index);

	for (int i = 0; i < _packet->listCount; i++)
	{
		m_tileList.push_back(_packet->positionList[i]);
	}

	m_targetPosition = m_tileList.front();

	m_tileList.pop_front();

	m_info.state = (STATE)_packet->state;
	m_currentTime = 0;
}

void Monster::PathFindFailed()
{
	//MYDEBUG("[ %d monster : Failed ] \n", m_info.index);

	PathFindStart(m_targetTile, STATE::RETURN);
}

bool Monster::PathMove()
{
	float distance = VECTOR2(m_info.position - m_targetPosition).SqrMagnitude();

	//��ǥ ��ǥ�� ����
	if (distance <= 0.1f)
	{
		m_info.position = m_targetPosition;
		m_nowTile = m_fieldTilesData->GetTile(
			static_cast<int>(m_info.position.x),
			static_cast<int>(m_info.position.y));

		//���� ���� ���� üũ
		UpdateSector();

		//���� Ÿ�ϸ���Ʈ�� �ִٸ�
		if (m_tileList.size() > 0)
		{
			m_targetPosition = m_tileList.front();
			m_tileList.pop_front();

			MonsterPositionPacket* monsterPositionPacket =
				reinterpret_cast<MonsterPositionPacket*>(m_sendBuffer->
					GetBuffer(sizeof(MonsterPositionPacket)));

			monsterPositionPacket->monsterIndex = m_info.index;
			monsterPositionPacket->position = m_info.position;
			monsterPositionPacket->Init(SendCommand::Zone2C_MONSTER_MOVE, sizeof(MonsterPositionPacket));
			
			m_sendBuffer->Write(monsterPositionPacket->size);

			//MYDEBUG("my position : %f , %f \n", m_info.position.x, m_info.position.y);

			//MainThread::getSingleton()->AddToMonsterPacketQueue({ this, monsterPositionPacket });
			m_field->SectorSendAll(m_sector->GetRoundSectorsVec(),
				reinterpret_cast<char*>(monsterPositionPacket), monsterPositionPacket->size);
		}
		//���� Ÿ�ϸ���Ʈ�� ���ٸ�
		else
		{
			return false;
		}
	}
	return true;
}

void Monster::UpdateSector()
{
	//ó�� �����ƴ�.
	if(m_sector == nullptr)
	{
		m_sector = m_sectorManager->
			GetSector((int)m_info.position.x, (int)m_info.position.y);

		m_sector->GetMonsterList()->AddItem(this);

		//MYDEBUG("spawn : %d \n", m_sector->GetSectorNum());
	}
	else
	{
		Sector* nowSector = m_sectorManager->
			GetSector(m_nowTile->GetX(), m_nowTile->GetY());

		//���� ���Ϳ� ����Ǿ��ִ� ���� ���Ͱ� �ٸ�
		if (nowSector != m_sector)
		{
			Sector* prevSector = m_sector;
			if (prevSector != nullptr)
			{
				//printf("[ Exit Monster (Prev Sector) ]");
				prevSector->GetMonsterList()->DeleteItem(this);
			}

			m_sector = nowSector;
			m_sector->GetMonsterList()->AddItem(this);
		}

		//������ ó�� X

		/*Sector* prevSector = m_sector;

		//���� ���Ϳ� ����Ǿ��ִ� ���� ���Ͱ� ����
		if (prevSector == nowSector)
		{
			//ó�� X
			return;
		}

		if (prevSector != nullptr)
		{
			//printf("[ Exit Monster (Prev Sector) ]");
		}

		m_sector = nowSector;
		m_sector->GetMonsterList()->AddItem(this);*/
	}
}