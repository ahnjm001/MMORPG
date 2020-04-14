#pragma once
#include <time.h>

#include "User.h"
#include "Monster.h"
#include "packet.h"
#include "singletonBase.h"
#include "ThreadClass.h"
#include "SharedQueue.h"

#include "SessionManager.h"
#include "ZoneManager.h"

//=====================================================

//ServerLogicThread

//�ش� ������ ��� �������� ��Ŷó���� �����ϴ� ���� Ŭ����
//���� ��Ŷ���� �� ���� ��ģ��.

//=====================================================

class ServerLogicThread : public ThreadClass<ServerLogicThread>,
	public singletonBase<ServerLogicThread>
{
private:
	SessionManager*					m_sessionManager;
	ZoneManager*					m_zoneManager;

	SharedQueue<PacketQueuePair>	m_monsterPacketQueue;

	//�׽�Ʈ �ð� üũ��
	time_t start, end;

public:
	ServerLogicThread();
	~ServerLogicThread();

	void Init(SessionManager* _sessionManager,
			ZoneManager* _zoneManager);

	void LoopRun();

	void ParsingUser();
	void ParsingMonster();

	void EnterZone(User* _user, Packet* _packet);
	void EnterZoneSuccess(User* _user);
	void UpdateUserPosition(User* _user, Packet* _packet);
	void UserAttackFailed(User* _user, Packet* _packet);
	void UserAttack(User* _user, Packet* _packet);
	void UserRevive(User* _user);

	void MonsterAttack(Monster* _monster, Packet* _packet);

	void RegisterUser(User* _user, Packet* _packet);
	void LogInUser(User* _user, Packet* _packet);

	void UpdateUser(User* _user, Packet* _packet);

	SharedQueue<PacketQueuePair>* GetSharedQueue() { return &m_monsterPacketQueue; }
};