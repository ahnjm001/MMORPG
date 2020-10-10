#pragma once
#include "stdafx.h"

#include "../ServerLibrary/HeaderFiles/Utils.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/DoubleQueue.h"
#include "../ServerLibrary/HeaderFiles/CriticalSection.h"

#include "packet.h"
#include "UnitInfo.h"
#include "PathFinding.h"
#include "FieldTilesData.h"

//=====================================================

//Monster

//Monster ��ü�̴�.
//PacketQueuePair�� Singleton���� ����� MainThread�� �����Ͽ� Monster�� ���� ��Ŷ�� �����صδ� �뵵�� ����.
//PathFinding�� ��ӹ޾� ������ ��ã�⿡ �̿�ȴ�.

//=====================================================

class User;
class Field;
class SectorManager;
class Sector;
class MainThread;

class Monster : public PathFinding
{
private:
	bool					m_failed;

	MonsterInfo				m_info;
	MonsterData				m_data;

	VECTOR2					m_spawnPosition;

	Tile*					m_homeTile;
	Tile*					m_nowTile;
	Tile*					m_targetTile;
	VECTOR2					m_targetPosition;

	list<VECTOR2>			m_tileList;

	//========================================

	Field*					m_field;
	FieldTilesData*			m_fieldTilesData;
	SectorManager*			m_sectorManager;

	Sector*					m_sector;

	User*					m_target;

	//========================================

	CRITICAL_SECTION		m_cs;

	//========================================

	bool					m_isDeath;
	bool					m_isAttack;

	float					m_currentTime;
	float					m_maxTime;

public:
	Monster(Field* _field, FieldTilesData* _fieldTilesData, 
		SectorManager* _sectorManager, MonsterInfo& _info, MonsterData& _data);
	~Monster();

	void Reset();

	void Update();

	void Spawn();
	void Move();

	void Attack();
	Packet* Hit(User* _user, int _damage);

	void FSM();

	void PathFindStart(Tile* _targetTile, STATE _state);

	void PathFindSuccess(PathFindPacket_Success* _packet);
	void PathFindFailed();

	bool PathMove();

	void UpdateSector();

	bool IsFailed() { return m_failed; }

	MonsterInfo& GetInfo() { return m_info; }
	MonsterData& GetData() { return m_data; }
	Field* GetField() { return m_field; }
	Sector* GetSector() { return m_sector; }
	User* GetTarget() { return m_target; }

	bool IsDeath() { return m_isDeath; }
};

