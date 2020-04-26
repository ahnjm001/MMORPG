#pragma once
#include "stdafx.h"

#include "Unit.h"
#include "Session.h"

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

class Field;
class Sector;

class User : public Session, public Unit
{
private:
	INFO				m_basicInfo;

	Sector*				m_sector;

	Tile*				m_tile;

	//===============================================

	Field*				m_field;
	FieldTilesData*		m_fieldTilesData;

	bool				m_isTestClient;

public:
	User();
	~User();

	virtual void Init();
	virtual void OnConnect(SOCKET _socket);
	virtual void Disconnect();
	virtual void Reset();
	//����ִ��� üũ
	virtual void HeartBeatChecked();
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

	//������ ���� ������
	void SendInfo();
	//�׽�Ʈ��
	void TestClientEnterField(Field* _Field, int _fieldNum);
	//Field ���� ��
	void EnterField(Field *_field, int _fieldNum, VECTOR2 _spawnPosition);
	//User�� ������(�̵��� ȣ���)
	void SetPosition(Position& _position);

	bool LogInUser(LogInPacket* _packet);
	void RegisterUser(RegisterUserPacket* _packet);

	//�α��� �ߺ���(�̹� ��������)->����ó��
	void LogInDuplicated();
	void LogInSuccess();

	bool CompareSector(Sector* _sector);

	Tile* GetTile() { return m_tile; }
	INFO* GetInfo() { return &m_basicInfo; }
	Field* GetField() { return m_field; }
	void SetField (Field* _field) { m_field = _field; }

	Sector* GetSector() { return m_sector; }
	void SetSector(Sector* _sector) { m_sector = _sector; }

	bool GetIsDeath() { return m_basicInfo.unitInfo.state == STATE::DEATH; }

	bool GetIsTestClient() { return m_isTestClient; }
};												 

