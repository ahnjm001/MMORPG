#pragma once
#include "stdafx.h"

#pragma pack(push, 1)

enum STATE : unsigned short
{
	SPAWN,
	IDLE,
	MOVE,
	ATTACK,
	HIT,
	DEATH,
	//���Ϳ�
	PATROL,
	//���Ϳ�
	READY,
	//���Ϳ�
	RETURN
};

struct ZeroToMax
{
	int currentValue;
	int maxValue;

	ZeroToMax(int _current = 0, int _max = 0)
	{
		currentValue = _current;
		maxValue = _max;
	}
};

struct Position : VECTOR2
{
	VECTOR2 direction;

	void Init(VECTOR2& _vector)
	{
		direction = _vector;
	}
};

struct UnitInfo
{
	WORD fieldNum;

	int level;

	ZeroToMax hp;
	ZeroToMax mp;
	ZeroToMax exp;

	int atk;
	int def;

	Position position;

	STATE state = SPAWN;
	
	void SetState(STATE _state)
	{
		state = _state;
	}

	void SetHp(int _current, int _max)
	{
		hp.currentValue = _current;
		hp.maxValue = _max;
	}

	void SetMp(int _current, int _max)
	{
		mp.currentValue = _current;
		mp.maxValue = _max;
	}

	void SetExp(int _current, int _max)
	{
		exp.currentValue = _current;
		exp.maxValue = _max;
	}

	void SetUnitPosition(float _x, float _z)
	{
		position.x = _x;
		position.y = _z;
	}

	void SetUnitInfo(
		STATE _state,
		int _level,
		int _maxHp,
		int _currentHp,
		int _maxMp,
		int _currentMp,
		int _maxExp,
		int _currentExp,
		int _atk,
		int _def)
	{
		state = _state;
		level = _level;
		hp.maxValue = _maxHp;
		hp.currentValue = _currentHp;
		mp.maxValue = _maxMp;
		mp.currentValue = _currentMp;
		exp.maxValue = _maxExp;
		exp.currentValue = _currentExp;
		atk = _atk;
		def = _def;
	}

	void Reset()
	{
		state = SPAWN;
		level = 0;
		hp.maxValue = 0;
		hp.currentValue = 0;
		mp.maxValue = 0;
		mp.currentValue = 0;
		exp.maxValue = 0;
		exp.currentValue = 0;
		atk = 0;
		def = 0;
		position.x = 0;
		position.y = 0;
	}
};

struct MonsterInfo
{
	int index;
	WORD monsterType;

	STATE state;
	ZeroToMax hp;
	VECTOR2 position;
};

struct MonsterData
{
	WORD monsterType;

	ZeroToMax hp;

	float attackDelay; //���� ������
	int attackDamage; //���� ������
	float attackDistance; //���� ��Ÿ�

	float returnDistance; //��ȯ �Ÿ�

	float moveSpeed; //������ �ӵ�
	int patrolRange; //���� ����
	float patrolDelay; //���� ������

	int dropExp;
};

#pragma pack(pop)