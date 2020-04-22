#pragma once
#include <map>
#include "ThreadClass.h"
#include "Monster.h"
#include "ZoneTilesData.h"
#include "SectorManager.h"

using namespace std;

//=====================================================

//MonsterLogicThread

//Zone�� ��� Monster�� �ൿ�� �����ϴ� Thread

//=====================================================

class Zone;

class MonsterLogicThread : public ThreadClass<MonsterLogicThread>
{
private:
	Zone* m_zone;
	ZoneTilesData* m_zoneTilesData;
	SectorManager* m_sectorManager;

	//���ʹ� ���� ã�ƾ��ϱ��ϹǷ� map���� �����.
	map<int, Monster*> m_monsterMap;

public:
	MonsterLogicThread();
	~MonsterLogicThread();

	void Init(Zone* _zone, ZoneTilesData* _zoneTilesData, SectorManager* _sectorManager);

	void LoopRun();

	void SendMonsterList(User* _user);
	Monster* GetMonster(int _num);
};

