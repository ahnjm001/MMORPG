#include "MainThread.h"

MainThread::MainThread()
{
}

MainThread::~MainThread()
{
	for (int i = MAX_EVENT; i > 0 ; --i)
	{
		if (m_hEvent[i-1]) { CloseHandle(m_hEvent[i-1]); m_hEvent[i-1] = 0; }
	}
}

bool MainThread::Init()
{
	for (int i = 0; i < MAX_EVENT; ++i)
	{
		//����Ȯ��?
		m_hEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);

		if (m_hEvent[i] == NULL)
		{
			int num = WSAGetLastError();
			MYDEBUG("[Event Handle Creating Fail - %d Error]\n", num);
			return false;
		}
	}

	processFunc[0] = &MainThread::ProcessUserPacket;
	processFunc[1] = &MainThread::ConnectUser;
	processFunc[2] = &MainThread::DisConnectUser;
	processFunc[3] = &MainThread::ProcessMonster;
	processFunc[4] = &MainThread::ProcessDBConnectorPacket;
	processFunc[5] = &MainThread::ProcessLogInServerPacket;
	processFunc[6] = &MainThread::ProcessPathFindAgentPacket;
	processFunc[7] = &MainThread::AddToHashMap;
	processFunc[8] = &MainThread::HeartBeat_DBAgent;
	processFunc[9] = &MainThread::HeartBeat_LoginServer;

	return true;
}

void MainThread::SetManagers(UserManager* _userManager,
	FieldManager* _fieldManager)
{
	m_userManager = _userManager;
	m_fieldManager = _fieldManager;

	m_packetHandler = new PacketHandler(*_userManager, *_fieldManager);

	m_threadSchedular = new ThreadSchedular();
	//m_threadSchedular->CreateSchedule(m_hEvent[EVENT_LOGIN_HEARTBEAT], 10000);
	//m_threadSchedular->CreateSchedule(m_hEvent[EVENT_DB_HEARTBEAT], 10000);
	m_threadSchedular->CreateSchedule(m_hEvent[EVENT_MONSTER], 1000 / 10);

	Thread<MainThread>::Start(this);

	MYDEBUG("[ MainThread Start ]\n");
}

//void MainThread::SetManagers(std::unique_ptr<UserManager>&& _userManager,
//	std::unique_ptr<FieldManager>&& _fieldManager)
//{
//	m_userManager = std::move(_userManager);
//	m_fieldManager = std::move(_fieldManager);
//
//	m_packetHandler = new PacketHandler(m_userManager, m_fieldManager);
//
//	Thread<MainThread>::Start(this);
//	
//	MYDEBUG("[ MainThread Start ]\n");
//}

void MainThread::LoopRun()
{
	while(1)
	{
		int num = WaitForMultipleObjects(MAX_EVENT, m_hEvent, FALSE, INFINITE);
		//printf("hEvent : %d \n", num);

		(this->*processFunc[num])();
	}
}

void MainThread::ProcessUserPacket()
{
	m_userPacketQueue.Swap();

	std::queue<PacketQueuePair_User>& userPacketQueue = m_userPacketQueue.GetSecondaryQueue();

	size_t size = userPacketQueue.size();

	for (int i = 0; i < size; i++)
	{
		const PacketQueuePair_User& PacketQueuePair_User = userPacketQueue.front();
		Packet* packet = PacketQueuePair_User.packet;
		User* user = PacketQueuePair_User.user;

		//�����۰����� �ݰ� �����?
		//m_isActived true���� false������ üũ�ϰ� Log�� �����.
		if (!user->IsConnected())
		{
			//MYDEBUG("Check - DisConnected\n");
			LOG::FileLog("../LogFile.txt", __FILENAME__, __LINE__, 
				"%d user disconnected but packet is existed", 
				user->GetInfo()->userInfo.userID);

			continue;
		}

		//�׽�Ʈ�� ��Ŷ ó��
		if (packet->cmd == 12345)
		{
			MYDEBUG("Test Checking \n");
		}
		else
		{
			m_packetHandler->HandleUserPacket(user, packet);
		}

		userPacketQueue.pop();
	}
}

void MainThread::ConnectUser()
{
	//������ Connect�� ���
	m_connectQueue.Swap();

	std::queue<SOCKET>& connectQueue = m_connectQueue.GetSecondaryQueue();

	size_t size = connectQueue.size();

	for (int i = 0; i < size; i++)
	{
		User* tempUser = m_userManager->PopUser();

		if (tempUser == nullptr)
		{
			//����ó��
			MYDEBUG("[ UserSession is nullptr ] \n");
			FILELOG("UserSession is nullptr");

			// + LOG �����
			//���� ���������ϱ� ��Ŷ �ϳ� ��������. Ȥ�� �ð� ������ ?
			continue;
		}

		tempUser->SetSocket(connectQueue.front());

		tempUser->OnConnect();

		m_userManager->AddTempuser(tempUser);

		//MYDEBUG("tempUserList Size : %zd \n", m_userManager->GetUserList()->GetItemList().size());

		connectQueue.pop();
	}
}

void MainThread::DisConnectUser()
{
	//������ Disconnect�� ���
	m_disconnectQueue.Swap();

	std::queue<User*>& disconnectQueue = m_disconnectQueue.GetSecondaryQueue();

	size_t size = disconnectQueue.size();

	for (int i = 0; i < size; i++)
	{
		User* tempUser = disconnectQueue.front();

		//���� �־��ٸ� �ش� ������ ���� ���� ��
		if (tempUser->GetField() != nullptr)
		{
			tempUser->GetField()->ExitUser(tempUser);
		}

		if (tempUser->m_isInHashMap)
		{
			//���ǸŴ������� ������ �������ܰ� ���ÿ� ������ƮǮ�� ��ȯ���ش�.
			m_userManager->ReturnUser(tempUser);
			//MYDEBUG("hashmapSize : %zd \n", m_userManager->GetUserHashMap()->GetItemHashMap().size());
		}
		else
		{
			m_userManager->DeleteTempUser(tempUser, true);
			//MYDEBUG("tempUserList Size : %zd \n", m_userManager->GetUserList()->GetItemList().size());
		}

		disconnectQueue.pop();
	}
}

void MainThread::ProcessMonster()
{
	map<WORD, Field*>& tempFieldMap = m_fieldManager->GetFieldMap();

	for (const auto& pair : tempFieldMap )
	{
		Field* field = pair.second;

		//�׽�Ʈ��
		/*if (field->GetFieldNum() == 3)
		{
			field->GetMonsterManager()->Update();
		}*/

		//�ʵ忡 ������ ������ �������� �ʴ´�.
		if (field->GetUserList()->GetItemList().size() <= 0) continue;

		field->GetMonsterManager()->Update();
	}
}

void MainThread::ProcessDBConnectorPacket()
{
	//if (!Connector->GetIsConnected())
	//{
	//	//������
	//	Connector->Connect();
	//}

	m_dbPacketQueue.Swap();

	std::queue<Packet*>& dbPacketQueue = m_dbPacketQueue.GetSecondaryQueue();

	size_t size = dbPacketQueue.size();

	for (int i = 0; i < size; i++)
	{
		Packet* packet = dbPacketQueue.front();

		m_packetHandler->HandleDBConnectorPacket(packet);

		dbPacketQueue.pop();
	}
}

void MainThread::ProcessLogInServerPacket()
{
	m_logInServerPacketQueue.Swap();

	std::queue<Packet*>& logInServerQueue = m_logInServerPacketQueue.GetSecondaryQueue();

	size_t size = logInServerQueue.size();

	for (int i = 0; i < size; i++)
	{
		Packet* packet = logInServerQueue.front();

		m_packetHandler->HandleLogInServerPacket(packet);

		logInServerQueue.pop();
	}
}

void MainThread::ProcessPathFindAgentPacket()
{
	m_pathFindAgentPacketQueue.Swap();

	std::queue<Packet*>& pathFindQueue = m_pathFindAgentPacketQueue.GetSecondaryQueue();

	size_t size = pathFindQueue.size();

	for (int i = 0; i < size; i++)
	{
		Packet* packet = pathFindQueue.front();

		m_packetHandler->HandlePathFindAgentPacket(packet);

		pathFindQueue.pop();
	}
}

void MainThread::AddToHashMap()
{
	m_hashMapQueue.Swap();

	std::queue<User*>& hashMapQueue = m_hashMapQueue.GetSecondaryQueue();

	size_t size = hashMapQueue.size();

	for (int i = 0; i < size; i++)
	{
		User* tempUser = hashMapQueue.front();

		m_userManager->DeleteTempUser(tempUser, false);
		m_userManager->AddUser(tempUser);
		tempUser->m_isInHashMap = true;

		//MYDEBUG("tempUserList Size : %zd \n", m_userManager->GetUserList()->GetItemList().size());
		//MYDEBUG("hashmapSize : %zd \n", m_userManager->GetUserHashMap()->GetItemHashMap().size());

		hashMapQueue.pop();
	}
}

void MainThread::HeartBeat_DBAgent()
{
	m_packetHandler->SendHeartBeat_DBAgent();
}

void MainThread::HeartBeat_LoginServer()
{
	m_packetHandler->SendHeartBeat_LoginServer();
}

void MainThread::AddToUserPacketQueue(const PacketQueuePair_User& _userPacketQueuePair)
{
	//2���� �ӵ� üũ
	//m_userPacketQueue.GetPrimaryQueue().emplace(this, packet);
	m_userPacketQueue.AddObject(_userPacketQueuePair);

	SetEvent(m_hEvent[EVENT_RECV]);
}

void MainThread::AddToConnectQueue(SOCKET _socket)
{
	m_connectQueue.AddObject(_socket);

	SetEvent(m_hEvent[EVENT_CONNECT]);
}

void MainThread::AddToDisConnectQueue(User* _user)
{
	m_disconnectQueue.AddObject(_user);

	SetEvent(m_hEvent[EVENT_DISCONNECT]);
}

void MainThread::AddToDBConnectorPacketQueue(Packet* _packet)
{
	m_dbPacketQueue.AddObject(_packet);

	SetEvent(m_hEvent[EVENT_DBCONNECTOR]);
}

void MainThread::AddToLogInServerPacketQueue(Packet* _packet)
{
	m_logInServerPacketQueue.AddObject(_packet);

	SetEvent(m_hEvent[EVENT_LOGINSERVER]);
}

void MainThread::AddToPathFindAgentPacketQueue(Packet* _packet)
{
	m_pathFindAgentPacketQueue.AddObject(_packet);

	SetEvent(m_hEvent[EVENT_PATHFINDAGENT]);
}

void MainThread::AddToHashMapQueue(User* _user)
{
	m_hashMapQueue.AddObject(_user);

	SetEvent(m_hEvent[EVENT_STOREUSER]);
}