#pragma once
#include <time.h>

#include "../ServerLibrary/HeaderFiles/FileLog.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/SingletonBase.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/Thread.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/DoubleQueue.h"

#include "User.h"
#include "Monster.h"
#include "packet.h"
#include "SessionManager.h"
#include "FieldManager.h"

//=====================================================

//ServerLogicThread

//�ش� ������ ��� �������� ��Ŷó���� �����ϴ� ���� Ŭ����
//���� ��Ŷ���� �� ���� ��ģ��.

//=====================================================
class ServerLogicThread : public Thread<ServerLogicThread>,
						  public SingletonBase<ServerLogicThread>
{
public  :
	enum eHandle : int
	{
		EVENT_RECV,
		EVENT_SEND,
		EVENT_CONNECT,
		EVENT_DISCONNECT,
		EVENT_MONSTER,
		EVENT_DBCONNECTOR,
		MAX_EVENT
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

	struct PacketQueuePair_Monster
	{
		class Monster* monster;
		Packet* packet;

		PacketQueuePair_Monster(Monster* _monster, Packet* _packet)
		{
			monster = _monster;
			packet = _packet;
		}
	};

private:
	SessionManager*							m_sessionManager;
	FieldManager*							m_fieldManager;

	DoubleQueue<PacketQueuePair_User>		m_userPacketQueue;
	DoubleQueue<Packet>						m_dbPacketQueue;
	DoubleQueue<PacketQueuePair_Monster>	m_monsterPacketQueue;
	DoubleQueue<SOCKET>						m_connectQueue;
	DoubleQueue<Session>					m_disconnectQueue;

	HANDLE									m_hEvent[MAX_EVENT];

	//�׽�Ʈ �ð� üũ��
	time_t start, end;

public:
	ServerLogicThread();
	~ServerLogicThread();

	bool Init();
	void GetManagers(SessionManager* _sessionManager,
		FieldManager* _fieldManager);

	void LoopRun();

	void ProcessUserPacket();
	void ProcessMonsterPacket();
	void ProcessDBConnectorPacket();
	void ConnectUser();
	void DisConnectUser();

	void AddToUserPacketQueue(PacketQueuePair_User* _userPacketQueuePair);
	void AddToDBConnectorPacketQueue(Packet* _packet);
	void AddToMonsterPacketQueue(PacketQueuePair_Monster* _monsterPacketQueuePair);
	void AddToConnectQueue(SOCKET _socket);
	void AddToDisConnectQueue(Session* _session);

	void OnPacket_EnterField(User* _user, Packet* _packet);
	void OnPacket_EnterFieldSuccess(User* _user);
	void OnPacket_UpdateUserPosition(User* _user, Packet* _packet, bool _isFinish);
	void OnPacket_UserAttackFailed(User* _user, Packet* _packet);
	void OnPacket_UserAttack(User* _user, Packet* _packet);
	void OnPacket_UserRevive(User* _user);

	void OnPacket_MonsterAttack(Monster* _monster, Packet* _packet);

	void OnPacket_RegisterUser(User* _user, Packet* _packet);
	void OnPacket_LogInUser(User* _user, Packet* _packet);
	void OnPacket_UpdateUser(User* _user, Packet* _packet);

	//DB agent====================================================
	void OnPacket_LogInSuccess(Packet* _packet);
	void OnPacket_LogInFailed(Packet* _packet);

	void OnPacket_RegisterSuccess(Packet* _packet);
	void OnPacket_RegisterFailed(Packet* _packet);

	void OnPacket_GetUserInfoSuccess(Packet* _packet);
	void OnPacket_GetUserInfoFailed(Packet* _packet);

	void OnPacket_UpdateUserSuccess(Packet* _packet);
	void OnPacket_UpdateUserFailed(Packet* _packet);

	Session* FindUser(SOCKET _socket);
	//============================================================

	void OnPacket_EnterTestUser(User* _user, Packet* _packet);
};