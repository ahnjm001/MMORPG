#pragma once
#include <time.h>

#include "User.h"
#include "Monster.h"
#include "packet.h"
#include "singletonBase.h"
#include "ThreadClass.h"
#include "SharedQueue.h"

#include "SessionManager.h"
#include "FieldManager.h"

//=====================================================

//ServerLogicThread

//해당 서버의 모든 유저들의 패킷처리를 관리하는 단일 클래스
//몬스터 패킷또한 이 곳을 거친다.

//=====================================================

class ServerLogicThread : public ThreadClass<ServerLogicThread>,
	public singletonBase<ServerLogicThread>
{
private:
	SessionManager*					m_sessionManager;
	FieldManager*					m_fieldManager;

	SharedQueue<PacketQueuePair>	m_monsterPacketQueue;

	//테스트 시간 체크용
	time_t start, end;

public:
	ServerLogicThread();
	~ServerLogicThread();

	void Init(SessionManager* _sessionManager,
		FieldManager* _fieldManager);

	void LoopRun();

	void ParsingUser();
	void ParsingMonster();

	void OnPacket_EnterField(User* _user, Packet* _packet);
	void OnPacket_EnterFieldSuccess(User* _user);
	void OnPacket_UpdateUserPosition(User* _user, Packet* _packet);
	void OnPacket_UserAttackFailed(User* _user, Packet* _packet);
	void OnPacket_UserAttack(User* _user, Packet* _packet);
	void OnPacket_UserRevive(User* _user);

	void OnPacket_MonsterAttack(Monster* _monster, Packet* _packet);

	void OnPacket_RegisterUser(User* _user, Packet* _packet);
	void OnPacket_LogInUser(User* _user, Packet* _packet);

	void OnPacket_UpdateUser(User* _user, Packet* _packet);

	void OnPacket_EnterTestUser(User* _user, Packet* _packet);
	//void OnPacket_MoveTestUser(User* _user, Packet* _packet);

	SharedQueue<PacketQueuePair>* GetSharedQueue() { return &m_monsterPacketQueue; }
};