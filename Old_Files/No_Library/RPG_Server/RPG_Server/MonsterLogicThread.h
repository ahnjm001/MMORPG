#pragma once
#include <map>
#include "ThreadClass.h"
#include "Monster.h"
#include "FieldTilesData.h"
#include "SectorManager.h"

using namespace std;

//=====================================================

//MonsterLogicThread

//Field�� ��� Monster�� �ൿ�� �����ϴ� Thread

//=====================================================

class Field;

class MonsterLogicThread : public ThreadClass<MonsterLogicThread>
{
private:
	Field* m_field;
	FieldTilesData* m_fieldTilesData;
	SectorManager* m_sectorManager;

	//���ʹ� ���� ã�ƾ��ϱ��ϹǷ� map���� �����.
	map<int, Monster*> m_monsterMap;

public:
	MonsterLogicThread();
	~MonsterLogicThread();

	void Init(Field* _field, FieldTilesData* _fieldTilesData, SectorManager* _sectorManager);

	void LoopRun();

	void SendMonsterList(User* _user);
	void SendMonsterList_InRange(User* _user);
	Monster* GetMonster(int _num);
};

