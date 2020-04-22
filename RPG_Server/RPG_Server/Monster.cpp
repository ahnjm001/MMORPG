#include "Monster.h"
#include "User.h"
#include "Zone.h"
#include "SectorManager.h"
#include "Sector.h"

Monster::~Monster()
{
}

void Monster::Init(MonsterInfo& _info, MonsterData& _data)
{
	m_info = _info;
	m_data = _data;
	m_info.hp = m_data.hp;

	m_spawnPosition.x = m_info.position.x;
	m_spawnPosition.y = m_info.position.y;

	m_sendBuffer = new SendBuffer();
	m_sendBuffer->Init(1000);

	m_currentTime = m_maxTime = 0.0f;

	Spawn();
}

void Monster::Reset()
{
	m_sendBuffer->Reset();
}

void Monster::Update()
{
	m_currentTime += 1.0f / 10.0f;

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
	m_homeTile = m_zoneTilesData->GetTile(
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
	monsterInfoPacket->Init(SendCommand::MONSTER_INFO, sizeof(MonsterInfoPacket));

	m_sendBuffer->Write(monsterInfoPacket->size);
	packetQueue.AddItem(PacketQueuePair(this, monsterInfoPacket));
}

void Monster::Move()
{
	VECTOR2 tempVec2 = VECTOR2(m_targetPosition - m_info.position);
	float magnitude = tempVec2.Magnitude();

	if (magnitude <= m_data.moveSpeed * (1.0f / 30.0f) || magnitude == 0.0f)
		m_info.position = m_targetPosition;

	//�� ����, ������ ����(���̰�,ȸ����) ���ϱ�
	//https://darkpgmr.tistory.com/121
	//VECTOR2(m_targetPosition - m_info.position) / magnitude �� �κ�

	//https://www.youtube.com/watch?v=WxWJorOVIj8
	//https://www.youtube.com/watch?v=-gokuNjpyNg

	m_info.position = m_info.position + tempVec2 / magnitude * m_data.moveSpeed * (1.0f / 30.0f);
	//���� * �̵��ӵ� * �ð�
}

void Monster::Attack()
{
	float magnitude = VECTOR2(m_info.position - m_target->GetInfo()->unitInfo.position).Magnitude();

	//�Ӵϱ� ����
	if (magnitude > m_data.attackDistance * m_data.attackDistance)
	{
		m_targetTile = m_target->GetTile();

		if (m_targetTile == m_nowTile) return;

		PathFindStart(m_targetTile);
		m_info.state = STATE::MOVE;

		return;
	}
	if (m_target->GetInfo()->unitInfo.zoneNum != m_zone->GetZoneNum() 
		|| m_target->GetIsDeath())
	{
		m_targetTile = m_homeTile;
		PathFindStart(m_targetTile);
		m_info.state = STATE::RETURN;

		return;
	}

	if (m_currentTime >= m_data.attackDelay)
	{
		MonsterAttackPacket* monsterAttackPacket =
			reinterpret_cast<MonsterAttackPacket*>(m_sendBuffer->
				GetBuffer(sizeof(MonsterAttackPacket)));

		monsterAttackPacket->monsterIndex = m_info.index;
		monsterAttackPacket->damage = m_data.attackDamage;
		monsterAttackPacket->Init(SendCommand::MONSTER_ATTACK, sizeof(MonsterAttackPacket));

		m_sendBuffer->Write(monsterAttackPacket->size);
		packetQueue.AddItem(PacketQueuePair(this, monsterAttackPacket));

		m_currentTime = 0.0f;
	}
}

//���� ������ ���ÿ� ������ ��� ����ȭ ���� �߻�
//ó���ؾ���.
Packet* Monster::Hit(User* _user, int _damage)
{
	m_locker.EnterLock();

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
		monsterHitPacket->Init(SendCommand::MONSTER_HIT_FAIL, sizeof(MonsterHitPacket));

		m_sendBuffer->Write(monsterHitPacket->size);
		return monsterHitPacket;

		m_locker.LeaveLock();
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
	monsterHitPacket->Init(SendCommand::MONSTER_HIT, sizeof(MonsterHitPacket));

	m_sendBuffer->Write(monsterHitPacket->size);

	if (m_info.hp.currentValue <= 0)
	{
		m_isDeath = true;
	}

	m_locker.LeaveLock();

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
				int randX = (rand() % m_data.patrolRange * 2) - m_data.patrolRange;
				int randY = (rand() % m_data.patrolRange * 2) - m_data.patrolRange;

				m_targetTile = m_zoneTilesData->
					GetTile(m_homeTile->GetX() + randX, m_homeTile->GetY() + randY);
			}

			PathFindStart(m_targetTile);
			m_info.state = STATE::PATROL;
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
			if (magnitude_1 <= m_data.attackDistance * m_data.attackDistance)
			{
				m_info.state = STATE::ATTACK;
				break;
			}

			float magnitude_2 = VECTOR2(m_info.position - m_spawnPosition).Magnitude();

			//��ȯ�Ÿ����� �־������ ��ȯ
			if (magnitude_2 > m_data.returnDistance * m_data.returnDistance)
			{
				m_targetTile = m_homeTile;
				PathFindStart(m_targetTile);
				m_info.state = STATE::RETURN;

				break;
			}

			//Ÿ�� ������ �������� Ÿ��Ÿ���� �ٲ���� ��� ����
			if (m_targetTile != m_target->GetTile())
			{
				m_targetTile = m_target->GetTile();
				PathFindStart(m_targetTile);

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

void Monster::PathFindStart(Tile* _targetTile)
{
	PathFinding::PathFind(m_tileList, m_nowTile, _targetTile);

	if (m_tileList.size() == 0) return;

	m_targetPosition = *m_tileList.begin();
	m_tileList.erase(m_tileList.begin());
	m_currentTime = 0;
}

bool Monster::PathMove()
{
	float distance = VECTOR2(m_info.position - m_targetPosition).SqrMagnitude();

	//��ǥ ��ǥ�� ����
	if (distance <= 0.1f)
	{
		m_info.position = m_targetPosition;
		m_nowTile = m_zoneTilesData->GetTile(
			static_cast<int>(m_info.position.x),
			static_cast<int>(m_info.position.y));

		//���� Ÿ�ϸ���Ʈ�� �ִٸ�
		if (m_tileList.size() > 0)
		{
			m_targetPosition = *m_tileList.begin();
			m_tileList.erase(m_tileList.begin());

			//���� ���� ���� üũ
			if (!m_sector->SectorRangeCheck(m_nowTile))
			{
				UpdateSector();
			}

			MonsterPositionPacket* monsterPositionPacket =
				reinterpret_cast<MonsterPositionPacket*>(m_sendBuffer->
					GetBuffer(sizeof(MonsterPositionPacket)));

			monsterPositionPacket->monsterIndex = m_info.index;
			monsterPositionPacket->position = m_info.position;
			monsterPositionPacket->Init(SendCommand::MONSTER_MOVE, sizeof(MonsterPositionPacket));
			
			m_sendBuffer->Write(monsterPositionPacket->size);
			packetQueue.AddItem(PacketQueuePair(this, monsterPositionPacket));
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
	Sector* prevSector = m_sector;
	if (prevSector != nullptr)
	{
		//printf("[ Exit User (Prev Sector) ]");
		prevSector->Manager<Monster>::DeleteItem(this);
	}

	m_sector = m_sectorManager->
		GetSector(m_info.position.x, m_info.position.y);

	m_sector->Manager<Monster>::AddItem(this);
}