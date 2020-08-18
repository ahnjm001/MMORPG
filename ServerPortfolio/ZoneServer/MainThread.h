#pragma once
#include <time.h>

#include "../ServerLibrary/HeaderFiles/FileLog.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/Thread.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/DoubleQueue.h"

#include "User.h"
#include "Monster.h"
#include "packet.h"
#include "UserManager.h"
#include "FieldManager.h"

#include "PacketHandler.h"

//=====================================================

//MainThread

//�ش� ������ ��� �������� ��Ŷó���� �����ϴ� ���� Ŭ����
//���� ��Ŷ���� �� ���� ��ģ��.

//Singleton�� ����ϴٰ� ���� ���� ����
//���� ����� ����� �Ǽ��� �����ϱ� ����.

//=====================================================

class HeartBeatThread;

class MainThread : public Thread<MainThread>
{
public:
	static MainThread* getSingleton()
	{
		static MainThread singleton;

		return &singleton;
	}

	enum eHandle : int
	{
		EVENT_RECV,
		EVENT_CONNECT,
		EVENT_DISCONNECT,
		EVENT_MONSTER,
		EVENT_DBCONNECTOR,
		EVENT_STOREUSER,
		EVENT_DELETE_TEMPUSER,
		EVENT_HEARTBEAT,
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
	UserManager*							m_userManager;
	FieldManager*							m_fieldManager;
	/*std::shared_ptr<UserManager>			m_userManager;
	std::shared_ptr<FieldManager>			m_fieldManager;*/

	PacketHandler*							m_packetHandler;
	HeartBeatThread*						m_heartBeatThread;

	DoubleQueue<PacketQueuePair_User>		m_userPacketQueue;
	DoubleQueue<PacketQueuePair_Monster>	m_monsterPacketQueue;
	DoubleQueue<Packet*>					m_dbPacketQueue;
	DoubleQueue<SOCKET>						m_connectQueue;
	DoubleQueue<User*>						m_disconnectQueue;
	DoubleQueue<User*>						m_hashMapQueue;
	DoubleQueue<User*>						m_deleteTempUserQueue;

	HANDLE									m_hEvent[MAX_EVENT];

	MainThread();

public:
	~MainThread();

	bool Init();
	void SetManagers(UserManager* _userManager,
		FieldManager* _fieldManager, HeartBeatThread* _heartBeatThread);
	//������ �ڿ��̵��� ���� rvalue reference�� �޴´�.
	/*void SetManagers(std::unique_ptr<UserManager>&& _userManager,
		std::unique_ptr<FieldManager>&& _fieldManager);*/

	void LoopRun();

	using Process = void (MainThread::*)();
	Process processFunc[MAX_EVENT];

	void ProcessUserPacket();
	void ProcessMonsterPacket();
	void ProcessDBConnectorPacket();
	void ConnectUser();
	void DisConnectUser();
	void AddToHashMap();
	void DeleteTempUser();
	void HeartBeat();

	void AddToUserPacketQueue(const PacketQueuePair_User& _userPacketQueuePair);
	void AddToMonsterPacketQueue(const PacketQueuePair_Monster& _monsterPacketQueuePair);
	void AddToDBConnectorPacketQueue(Packet* _packet);
	void AddToConnectQueue(SOCKET _socket);
	void AddToDisConnectQueue(User* _user);
	void AddToHashMapQueue(User* _user);
	void AddToDeleteTempUserQueue(User* _user);
	void HearBeatCheck();
};