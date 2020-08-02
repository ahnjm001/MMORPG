#include "MainThread.h"

MainThread::MainThread()
{
}

MainThread::~MainThread()
{
	for (int i = MAX_EVENT; i > 0 ; --i)
	{
		if (m_hEvent[i-1]) {	CloseHandle(m_hEvent[i-1]); m_hEvent[i-1] = 0; 	}
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

	return true;
}

void MainThread::SetManagers(UserManager* _userManager,
	FieldManager* _fieldManager)
{
	m_userManager = _userManager;
	m_fieldManager = _fieldManager;

	m_packetHandler = new PacketHandler(*_userManager, *_fieldManager);

	Thread<MainThread>::Start(this);

	MYDEBUG("[ MainThread Start ]\n");
}

void MainThread::LoopRun()
{
	while(1)
	{
		int num = WaitForMultipleObjects(MAX_EVENT, m_hEvent, FALSE, INFINITE);
		//printf("hEvent : %d \n", num);

		switch (num)
		{
		case EVENT_RECV:
		{
			m_userPacketQueue.Swap();

			ProcessUserPacket();
		}	
			break;
		case EVENT_DBCONNECTOR:
		{
			m_dbPacketQueue.Swap();

			ProcessDBConnectorPacket();
		}
			break;
		case EVENT_MONSTER:
		{
			m_monsterPacketQueue.Swap();

			ProcessMonsterPacket();
		}
			break;
		case EVENT_CONNECT:
		{
			//������ Connect�� ���
			m_connectQueue.Swap();

			ConnectUser();
		}			
			break;
		case EVENT_DISCONNECT:
		{
			//������ Disconnect�� ���
			m_disconnectQueue.Swap();

			DisConnectUser();
		}	
			break;
		case EVENT_STOREUSER:
		{
			m_storeQueue.Swap();

			StoreUser();
		}
			break;
		case EVENT_DELETE_TEMPUSER:
		{
			m_tempUserQueue.Swap();

			DeleteTempUser();
		}
			break;
		}
	}
}

void MainThread::ProcessUserPacket()
{
	std::queue<PacketQueuePair_User>& userPacketQueue = m_userPacketQueue.GetSecondaryQueue();

	size_t size = userPacketQueue.size();
	/*cout << m_userPacketQueue.GetPrimaryQueue().size() << " / " 
		<< m_userPacketQueue.GetSecondaryQueue().size() << endl;*/

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
			FILELOG("%d user disconnected but packet is existed", user->GetInfo()->userInfo.userID);

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

void MainThread::ProcessMonsterPacket()
{
	std::queue<PacketQueuePair_Monster>& monsterPacketQueue = m_monsterPacketQueue.GetSecondaryQueue();

	size_t size = monsterPacketQueue.size();

	for(int i = 0; i < size; i++)
	{
		const PacketQueuePair_Monster& PacketQueuePair_Monster = monsterPacketQueue.front();
		Packet* packet = PacketQueuePair_Monster.packet;
		Monster* monster = PacketQueuePair_Monster.monster;

		//�ش� ���Ͱ� �׾��ִٸ� �н�
		if (monster->IsDeath()) continue;

		m_packetHandler->HandleMonsterPacket(monster, packet);

		monsterPacketQueue.pop();
	}
}

void MainThread::ProcessDBConnectorPacket()
{
	//if (!Connector->GetIsConnected())
	//{
	//	//������
	//	Connector->Connect();
	//}

	std::queue<Packet*>& dbPacketQueue = m_dbPacketQueue.GetSecondaryQueue();

	size_t size = dbPacketQueue.size();

	for (int i = 0; i < size; i++)
	{
		Packet* packet = dbPacketQueue.front();

		m_packetHandler->HandleDBConnectorPacket(packet);

		dbPacketQueue.pop();
	}
}

void MainThread::ConnectUser()
{
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

		MYDEBUG("tempUserList Size : %d \n", m_userManager->GetItemList().size());

		connectQueue.pop();
	}
}

void MainThread::DisConnectUser()
{
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

		//���ǸŴ������� ������ �������ܰ� ���ÿ� ������ƮǮ�� ��ȯ���ش�.
		m_userManager->ReturnUser(tempUser);
		MYDEBUG("hashmapSize : %d \n", m_userManager->GetItemHashMap().size());

		disconnectQueue.pop();
	}
}

void MainThread::StoreUser()
{
	std::queue<User*>& storeQueue = m_storeQueue.GetSecondaryQueue();

	size_t size = storeQueue.size();

	for (int i = 0; i < size; i++)
	{
		User* tempUser = storeQueue.front();

		m_userManager->DeleteTempUser(tempUser, false);
		m_userManager->AddUser(tempUser);

		MYDEBUG("tempUserList Size : %d \n", m_userManager->GetItemList().size());
		MYDEBUG("hashmapSize : %d \n", m_userManager->GetItemHashMap().size());

		storeQueue.pop();
	}
}

void MainThread::DeleteTempUser()
{
	std::queue<User*>& tempUserQueue = m_tempUserQueue.GetSecondaryQueue();

	size_t size = tempUserQueue.size();

	for (int i = 0; i < size; i++)
	{
		User* tempUser = tempUserQueue.front();

		m_userManager->DeleteTempUser(tempUser, true);

		MYDEBUG("tempUserList Size : %d \n", m_userManager->GetItemList().size());

		tempUserQueue.pop();
	}
}

void MainThread::AddToUserPacketQueue(const PacketQueuePair_User& _userPacketQueuePair)
{
	m_userPacketQueue.AddObject(_userPacketQueuePair);

	SetEvent(m_hEvent[EVENT_RECV]);
}

void MainThread::AddToMonsterPacketQueue(const PacketQueuePair_Monster& _monsterPacketQueuePair)
{
	m_monsterPacketQueue.AddObject(_monsterPacketQueuePair);

	SetEvent(m_hEvent[EVENT_MONSTER]);
}

void MainThread::AddToDBConnectorPacketQueue(Packet* _packet)
{
	m_dbPacketQueue.AddObject(_packet);

	SetEvent(m_hEvent[EVENT_DBCONNECTOR]);
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

void MainThread::AddToStoreQueue(User* _user)
{
	m_storeQueue.AddObject(_user);

	SetEvent(m_hEvent[EVENT_STOREUSER]);
}

void MainThread::AddToTempUserQueue(User* _user)
{
	m_tempUserQueue.AddObject(_user);

	SetEvent(m_hEvent[EVENT_DELETE_TEMPUSER]);
}