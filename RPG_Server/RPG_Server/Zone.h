#pragma once
#include <time.h>
#include <algorithm>

#include "ManagerFrame.h"
#include "SendBuffer.h"

#include "User.h"
#include "ZoneTilesData.h"
#include "MonsterLogicThread.h"
#include "SectorManager.h"

//=====================================================

//Zone

//�� Zone�� ������ ������� ���͵��� ó���Ѵ�.

//=====================================================

class Zone : public Manager<User>
{
private:
	WORD m_zoneNum;
	VECTOR2 m_spawnPosition;

	//==================================================

	ZoneTilesData m_zoneTilesData;

	MonsterLogicThread m_monsterLogicThread;

	SectorManager m_sectorManager;

	CriticalSectionClass m_locker;

	//==================================================

	SendBuffer* m_sendBuffer;

	Sector*		m_exitSectors[5];
	Sector*		m_enterSectors[5];

	time_t		start, end;

public:
	Zone();
	~Zone();

	void Init(int _num, VECTOR2 _spawnPosition);
	void GetMap(const char* _name);
	void ZoneSendAll(char * _buffer, int _size);
	//�� ���͸� �������� �ֺ� 8���� ��ο��� SendAll���ִ� �뵵
	//ex) ���� ���� �������� �ֺ� ���� SendAll
	void SectorSendAll(Sector* _sector, Sector** _sectors, char * _buffer, int _size);
	//���͵鿡�� ���� SendAll���ִ� �뵵
	//ex) ���� ���� ���� �� �ش� ���� ���� �� ���͵鿡�� �������� �˷��ִ� SendAll
	void SectorSendAll(Sector** _sectors, char * _buffer, int _size);

	void SendUserList(User* _user);
	void SendUserList_InRange(User* _user);

	void EnterTestClient(User* _user);
	void EnterUser(User* _user);
	void SendEnterUserInfo(User* _user);

	void ExitUser(User* _user);
	void SendExitUserInfo(int _num);

	bool UserAttack(User * _user, int _monsterIndex);

	void RespawnUser(User* _user);

	void UpdateUserSector(User* _user);
	void ExitSector(User* _user, int _nowNum, int _prevNum, char* _packet, int _size);
	void SendInvisibleUserList(User* _user);
	void EnterSector(User* _user, int _nowNum, int _prevNum, char* _packet, int _size);
	void SendVisibleUserList(User* _user);

	WORD GetZoneNum() { return m_zoneNum; }

	VECTOR2& GetSpawnPosition() { return m_spawnPosition; }

	ZoneTilesData* GetTilesData() { return &m_zoneTilesData; }
};