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

	m_leaveSectorsVec.resize(9);
	m_enterSectorsVec.resize(9);

	Spawn();
};

Monster::~Monster()
{
	if(m_sendBuffer != nullptr) delete m_sendBuffer;

	m_tileList.clear();
	m_tileList.resize(0);

	m_leaveSectorsVec.clear();
	m_leaveSectorsVec.resize(0);

	m_enterSectorsVec.clear();
	m_enterSectorsVec.resize(0);
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

	//두 직선, 벡터의 관계(사이각,회전각) 구하기
	//https://darkpgmr.tistory.com/121
	//VECTOR2(m_targetPosition - m_info.position) / magnitude 이 부분

	//https://www.youtube.com/watch?v=WxWJorOVIj8
	//https://www.youtube.com/watch?v=-gokuNjpyNg

	m_info.position = m_info.position + tempVec2 / magnitude * m_data.moveSpeed * (1.0f / 10.0f);
	//각도 * 이동속도 * 시간
}

void Monster::Attack()
{
	float magnitude = VECTOR2(m_info.position - m_target->GetInfo()->unitInfo.position).Magnitude();

	//머니까 따라감
	if (magnitude > (m_data.attackDistance * 2) - m_data.attackDistance)
	{
		m_targetTile = m_target->GetTile();

		if (m_targetTile == m_nowTile) return;

		//MYDEBUG("따라감 \n");

		PathFindStart(m_targetTile, STATE::MOVE);
		//m_info.state = STATE::MOVE;

		return;
	}

	if (m_target->GetInfo()->unitInfo.fieldNum != m_field->GetFieldNum() 
		|| m_target->IsDeath())
	{
		//MYDEBUG("죽었으니 돌아가기 \n");

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

	//대부분 알피지는 이럴때 공격이 안됐음
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
		//기본 상태
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
			break;
		}
		//정찰
		case STATE::PATROL:
		{
			Move();
			//이동중이라면 계속 PATROL 고정시켜줌
			if (PathMove()) break;
			//이동이 끝났으니 IDLE로
			m_info.state = STATE::IDLE;
			m_currentTime = 0.0f;
			m_targetTile = nullptr;
			break;
		}
		//이동(공격시 멀 경우에 사용됨)
		case STATE::MOVE:
		{
			Move();

			float magnitude_1 = VECTOR2(m_info.position - m_target->GetInfo()->unitInfo.position).Magnitude();

			//사정거리 내면 공격
			if (magnitude_1 <= (m_data.attackDistance * 2) - m_data.attackDistance)
			{
				m_info.state = STATE::ATTACK;
				break;
			}

			float magnitude_2 = VECTOR2(m_info.position - m_spawnPosition).Magnitude();

			//귀환거리보다 멀어질경우 귀환
			if (magnitude_2 > (m_data.returnDistance * 2) - m_data.returnDistance)
			{
				//MYDEBUG("멀어서 귀환 \n");

				m_targetTile = m_homeTile;
				PathFindStart(m_targetTile, STATE::RETURN);
				//m_info.state = STATE::RETURN;

				break;
			}

			//타겟 유저가 움직여서 타겟타일이 바뀌었을 경우 추적
			if (m_targetTile != m_target->GetTile())
			{
				m_targetTile = m_target->GetTile();
				PathFindStart(m_targetTile, STATE::MOVE);

				break;
			}

			//이동중이 아니다 == 도착했다. -> 공격
			if (!PathMove())
			{
				m_info.state = STATE::ATTACK;
			}

			break;
		}
		//원래 타일로 돌아가기
		case STATE::RETURN:
		{
			Move();

			//이동 끝남
			if (!PathMove())
			{
				//체력 다시 원상태로
				Spawn();

				m_currentTime = 0.0f;
			}
			break;
		}
		//공격
		case STATE::ATTACK:
		{
			Attack();
			break;
		}
		//사망 - 5초 후 Spawn
		case STATE::DEATH:
		{
			if (m_currentTime >= 5.0f)
			{
				Spawn();

				m_currentTime = 0.0f;
			}

			break;
		}
		//생성 - 첫 생성 & 부활 둘 다 이용됨
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
	m_info.state = STATE::READY;

	m_tileList.clear();

	PathFinderAgent::getSingleton()->SendPathFindPacket(m_info.index, m_field->GetFieldNum(), _state,
		m_nowTile->GetX(), m_nowTile->GetY(), _targetTile->GetX(), _targetTile->GetY());

	/*PathFinding::PathFind(&m_tileList, m_nowTile, _targetTile);

	MYDEBUG("target : %d , %d \n", _targetTile->GetX(), _targetTile->GetY());

	if (m_tileList.size() == 0) return;

	m_targetPosition = m_tileList.front();

	m_tileList.pop_front();
	m_currentTime = 0;*/
}

void Monster::PathFindSuccess(PathFindPacket_Success* _packet)
{
	for (int i = 0; i < _packet->listCount; i++)
	{
		m_tileList.emplace_back(_packet->positionList[i]);
	}

	m_targetPosition = m_tileList.front();

	m_tileList.pop_front();

	m_info.state = (STATE)_packet->state;
	m_currentTime = 0;
}

void Monster::PathFindFailed()
{
	m_info.state = STATE::RETURN;
}

bool Monster::PathMove()
{
	float distance = VECTOR2(m_info.position - m_targetPosition).SqrMagnitude();

	//목표 좌표에 도착
	if (distance <= 0.1f)
	{
		m_info.position = m_targetPosition;
		m_nowTile = m_fieldTilesData->GetTile(
			static_cast<int>(m_info.position.x),
			static_cast<int>(m_info.position.y));

		//현재 섹터 범위 체크
		UpdateSector();

		//남은 타일리스트가 있다면
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
		//남은 타일리스트가 없다면
		else
		{
			return false;
		}
	}
	return true;
}

void Monster::UpdateSector()
{
	//처음 스폰됐다.
	if(m_sector == nullptr)
	{
		m_sector = m_sectorManager->
			GetSector(m_info.position.x, m_info.position.y);

		m_sector->GetMonsterList()->AddItem(this);

		//MYDEBUG("spawn : %d \n", m_sector->GetSectorNum());
	}
	else
	{
		Sector* nowSector = m_sectorManager->
			GetSector(m_nowTile->GetX(), m_nowTile->GetY());

		Sector* prevSector = m_sector;

		//현재 섹터와 저장되어있던 나의 섹터가 같음
		if (prevSector == nowSector)
		{
			//처리 X
			return;
		}

		if (prevSector != nullptr)
		{
			//printf("[ Exit Monster (Prev Sector) ]");

			/*prevSector->GetMonsterList()->DeleteItem(this);

			m_sector = nowSector;
			m_sector->GetMonsterList()->AddItem(this);

			std::vector<Sector*>& prevSectorsNeighbor = prevSector->GetRoundSectorsVec();
			std::vector<Sector*>& nowSectorsNeighbor = nowSector->GetRoundSectorsVec();
		
			auto iter1 = std::set_difference(
				prevSectorsNeighbor.begin(), prevSectorsNeighbor.end(),
				nowSectorsNeighbor.begin(), nowSectorsNeighbor.end(),
				m_leaveSectorsVec.begin());

			m_leaveSectorsVec.resize(iter1 - m_leaveSectorsVec.begin());

			auto iter2 = std::set_difference(
				nowSectorsNeighbor.begin(), nowSectorsNeighbor.end(),
				prevSectorsNeighbor.begin(), prevSectorsNeighbor.end(),
				m_enterSectorsVec.begin());

			m_enterSectorsVec.resize(iter2 - m_enterSectorsVec.begin());

			m_leaveSectorsVec.resize(9);
			m_enterSectorsVec.resize(9);*/
		}

		m_sector = nowSector;
		m_sector->GetMonsterList()->AddItem(this);
	}
}