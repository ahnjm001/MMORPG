#pragma once
#include "stdafx.h"

#include "Unit.h"
#include "Session.h"

#include "packet.h"

#include "ZoneTilesData.h"
#include "Sector.h"

//=====================================================

//User

//Session�� Unit�� ��ӹ޾� ���� Ŭ����

//=====================================================

struct INFO
{
	UserInfo	userInfo;
	UnitInfo	unitInfo;
};

class Zone;

class User : public Session, public Unit
{
private:
	INFO				m_basicInfo;

	Sector*				m_sector;

	Tile*				m_tile;

	//===============================================

	Zone*				m_zone;
	ZoneTilesData*		m_zoneTilesData;

	bool				m_startCheckingHeartBeat;
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
	void TestClientEnterZone(Zone* _zone, int _zoneNum);
	//Zone ���� ��
	void EnterZone(Zone* _zone, int _zoneNum, VECTOR2 _spawnPosition);
	//User�� ������ 
	void SetPosition(Position& _position);

	bool LogInUser(LogInPacket* _packet);
	void RegisterUser(RegisterUserPacket* _packet);

	//�α��� �ߺ���(�̹� ��������)->����ó��
	void LogInDuplicated();
	void LogInSuccess();

	Tile* GetTile() { return m_tile; }
	INFO* GetInfo() { return &m_basicInfo; }
	Zone* GetZone() { return m_zone; }
	void SetZone(Zone* _zone) { m_zone = _zone; }

	bool GetIsDeath() { return m_basicInfo.unitInfo.state == STATE::DEATH; }
	
	void StartCheckingHeartBeat() { m_startCheckingHeartBeat = true; }
	bool GetStartCheckingHeartBeat() { return m_startCheckingHeartBeat; }

	bool GetIsTestClient() { return m_isTestClient; }
};												 

