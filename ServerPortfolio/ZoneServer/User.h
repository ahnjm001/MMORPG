#pragma once
#include "stdafx.h"
#include"../ServerLibrary/HeaderFiles/OnlyHeaders/DoubleQueue.h"

#include "Unit.h"
#include "Session.h"
#include "PathFinding.h"

#include "packet.h"

#include "FieldTilesData.h"

//=====================================================

//User

//Session�� Unit�� ��ӹ޾� ���� Ŭ����

//=====================================================

struct INFO
{
	UserInfo	userInfo;
	UnitInfo	unitInfo;
};

struct PacketQueuePair_User
{
	class User* user;
	Packet* packet;

	PacketQueuePair_User(User* _user, Packet* _packet)
	{
		user = _user;
		packet = _packet;
	}
};

class Field;
class Sector;

class User : public Session, public Unit
{
private:
	INFO				m_basicInfo;

	Sector*				m_sector;

	Tile*				m_tile;

	Field*				m_field;
	FieldTilesData*		m_fieldTilesData;

	bool				m_isGetUserList;

	DoubleQueue<PacketQueuePair_User>&	m_doubleQueue;

	//===============================================
	bool				m_isTestClient;

public:
	User(DoubleQueue<PacketQueuePair_User>& _doubleQueue)
		: m_doubleQueue(_doubleQueue)
	{

	}

	~User();

	void Init(SOCKET _listenSocket, HANDLE _handle);
	void OnConnect();
	void Disconnect();
	void Reset();
	void CheckCompletion(ST_OVERLAPPED* _overlapped);
	void Parsing();

	//����ִ��� üũ
	void HeartBeatChecked();

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
	void RequestUserInfo();
	//������ ���� �ޱ� ���� -> ���� ���� ����(Ŭ�󿡰�)
	void SendInfo(GetSessionInfoPacket* _packet);
	//������ ���� �ޱ� ����
	void RequestUserInfoFailed();
	//Field ���� ��
	void EnterField(Field *_field, int _fieldNum, VECTOR2 _spawnPosition);
	//User�� ������(�̵��� ȣ���)
	void SetPosition(Position& _position);

	void LogInUser(LogInPacket* _packet);
	void RegisterUser(RegisterUserPacket* _packet);

	//�α��� �ߺ���(�̹� ��������)->����ó��
	void LogInDuplicated();
	void LogInSuccess(int _num);
	void LogInFailed();

	void RegisterSuccess();
	void RegisterFailed();

	bool CompareSector(Sector* _sector);

	//�׽�Ʈ��
	void TestClientEnterField(Field* _Field, int _fieldNum, int _dummyNum, VECTOR2 _spawnPosition);

	Tile* GetTile() { return m_tile; }
	INFO* GetInfo() { return &m_basicInfo; }

	Field* GetField() { return m_field; }
	void SetField (Field* _field) { m_field = _field; }

	Sector* GetSector() { return m_sector; }
	void SetSector(Sector* _sector) { m_sector = _sector; }

	bool IsDeath() { return m_basicInfo.unitInfo.state == STATE::DEATH; }

	bool IsTestClient() { return m_isTestClient; }

	void SetState(STATE _state) { m_basicInfo.unitInfo.state = _state; }

	bool GetIsGetUserList() { return m_isGetUserList; }
	void SetIsGetUserList(bool _bool) { m_isGetUserList = _bool; }
};												 

