#pragma once
#include <chrono>

#include "stdafx.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/DoubleQueue.h"
#include "../ServerLibrary/HeaderFiles/ClientSession.h"

#include "UnitInfo.h"
#include "PathFinding.h"
#include "FieldTilesData.h"

#include "packet.h"

//=====================================================

//User

//Session�� Unit�� ��ӹ޾� ���� Ŭ����

//=====================================================

struct INFO
{
	UserInfo	userInfo;
	UnitInfo	unitInfo;
};

class Field;
class Sector;

class User : public ClientSession
{
public:
	bool				m_isInHashMap;

	bool				m_isAlreadyDisConnected;

private:
	INFO				m_basicInfo;

	Tile*				m_tile;

	Sector*				m_sector;

	Field*				m_field;
	FieldTilesData*		m_fieldTilesData;

	//===============================================
	bool				m_isGetUserList;

public:
	User();
	~User();

	void OnConnect();
	void DisConnect();
	void Reset();

	void OnRecv();

	//������ ������ DB�� �ø��� �Լ�
	void UpdateInfo();

	//����
	void Death();
	//��Ȱ �� ù����
	void Respawn(VECTOR2 _spawnPosition);
	//����
	void Hit(int _index, int _damage);

	//����ġ ȹ��
	void PlusExp(int _exp);
	//������
	void LevelUp();

	//������ ���� �ޱ��û(DB������)
	void RequestUserInfo(int _num);
	//������ ���� �ޱ� ���� -> ���� ���� ����(Ŭ�󿡰�)
	void SendInfo(GetSessionInfoPacket* _packet);
	//������ ���� �ޱ� ����
	void RequestUserInfoFailed();
	//Field ���� ��
	void EnterField(Field *_field, int _fieldNum, VECTOR2 _spawnPosition);
	//User�� ������(�̵��� ȣ���)
	void SetPosition(Position& _position);

	bool CompareSector(Sector* _sector);

	//�׽�Ʈ��
	void TestClientEnterField(Field* _Field, int _fieldNum, int _dummyNum, VECTOR2 _spawnPosition);

	Tile* GetTile() { return m_tile; }
	INFO* GetInfo() { return &m_basicInfo; }

	Field* GetField() { return m_field; }
	void SetField(Field* _field) { m_field = _field; }

	Sector* GetSector() { return m_sector; }
	void SetSector(Sector* _sector) { m_sector = _sector; }

	bool IsDeath() { return m_basicInfo.unitInfo.state == STATE::DEATH; }

	void SetState(STATE _state) { m_basicInfo.unitInfo.state = _state; }

	bool GetIsGetUserList() { return m_isGetUserList; }
	void SetIsGetUserList(bool _bool) { m_isGetUserList = _bool; }
};

