#include "ServerLogicThread.h"

#include "DBConnector.h"

ServerLogicThread::ServerLogicThread()
{
}

ServerLogicThread::~ServerLogicThread()
{
	for (int i = MAX_EVENT; i > 0 ; i--)
	{
		if (m_hEvent[i-1]) {	CloseHandle(m_hEvent[i-1]); m_hEvent[i-1] = 0; 	}
	}

	{
		while (m_userPacketQueue.GetPrimaryQueueSize() != 0)
		{
			delete m_userPacketQueue.PopObject();
		}
		while (m_dbPacketQueue.GetPrimaryQueueSize() != 0)
		{
			delete m_dbPacketQueue.PopObject();
		}
		while (m_monsterPacketQueue.GetPrimaryQueueSize() != 0)
		{
			delete m_monsterPacketQueue.PopObject();
		}
		while (m_connectQueue.GetPrimaryQueueSize() != 0)
		{
			delete m_connectQueue.PopObject();
		}
		while (m_disconnectQueue.GetPrimaryQueueSize() != 0)
		{
			delete m_disconnectQueue.PopObject();
		}

		m_userPacketQueue.Swap();
		m_dbPacketQueue.Swap();
		m_monsterPacketQueue.Swap();
		m_connectQueue.Swap();
		m_disconnectQueue.Swap();

		while (m_userPacketQueue.GetPrimaryQueueSize() != 0)
		{
			delete m_userPacketQueue.PopObject();
		}
		while (m_dbPacketQueue.GetPrimaryQueueSize() != 0)
		{
			delete m_dbPacketQueue.PopObject();
		}
		while (m_monsterPacketQueue.GetPrimaryQueueSize() != 0)
		{
			delete m_monsterPacketQueue.PopObject();
		}
		while (m_connectQueue.GetPrimaryQueueSize() != 0)
		{
			delete m_connectQueue.PopObject();
		}
		while (m_disconnectQueue.GetPrimaryQueueSize() != 0)
		{
			delete m_disconnectQueue.PopObject();
		}
	}
}

bool ServerLogicThread::Init()
{
	for (int i = 0; i < MAX_EVENT; i++)
	{
		//����Ȯ��?
		m_hEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);

		if (m_hEvent[i] == NULL)
		{
			int num = WSAGetLastError();

			printf("[ Event Handle Creating Fail - %d Error ]\n", num);

			return false;
		}
	}

	return true;
}

void ServerLogicThread::GetManagers(SessionManager* _sessionManager,
	FieldManager* _fieldManager)
{
	m_sessionManager = _sessionManager;
	m_fieldManager = _fieldManager;

	Thread<ServerLogicThread>::Start(this);

	printf("[ ServerLogicThread Start ]\n");
}

void ServerLogicThread::LoopRun()
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
		}
	}
}

void ServerLogicThread::ProcessUserPacket()
{
	while (1)
	{
		if (m_userPacketQueue.IsEmpty()) return;

		PacketQueuePair_User* PacketQueuePair_User = m_userPacketQueue.PopObject();
		Packet* packet = PacketQueuePair_User->packet;
		User* user = PacketQueuePair_User->user;

		if (!user->IsConnected())
		{
			printf("Check - DisConnected\n");

			continue;
		}

		//�׽�Ʈ�� ��Ŷ ó��
		if (packet->cmd == 12345)
		{
			printf("Test Checking \n");
		}
		else
		{
			switch (static_cast<RecvCommand>(packet->cmd))
			{
				//ä�� ��Ŷ
				//�ٷ� �ش� ���� SendAll
			case RecvCommand::C2Zone_CHATTING:
				user->GetField()->
					SectorSendAll(user->GetSector()->GetRoundSectorsVec(),
						reinterpret_cast<char*>(packet), packet->size);
				break;
				//HeartBeatChecked ���� ��Ŷ ���� ��
			case RecvCommand::C2Zone_CHECK_ALIVE:
				user->HeartBeatChecked();
				break;
				//ȸ������ �õ���
			case RecvCommand::C2Zone_REGISTER_ACCOUNT:
				OnPacket_RegisterUser(user, packet);
				break;
				//�α��� �õ���
			case RecvCommand::C2Zone_LOGIN:
				OnPacket_LogInUser(user, packet);
				break;
				//���� �� ���� ���� ��û ��
			case RecvCommand::C2Zone_REQUIRE_INFO:
				user->RequestUserInfo();
				break;
				//���� ���� �� ù �� ���� �� �޴� ��Ŷ
				//���� ��ġ, Ÿ�� ����, ���� �־��ٸ� �ش� �������� Exitó��
				//�ش� ���� ���� ���� �����鿡�� �����ߴٰ� �˷���
			case RecvCommand::C2Zone_TRY_ENTER_FIELD:
				OnPacket_EnterField(user, packet);
				break;
				//���� ���� ������ ����Ʈ ������
			case RecvCommand::C2Zone_ENTER_FIELD_SUCCESS:
				OnPacket_EnterFieldSuccess(user);
				break;
				//���� �̵�
			case RecvCommand::C2Zone_USER_MOVE:
				OnPacket_UpdateUserPosition(user, packet, false);
				break;
				//���� �̵�
			case RecvCommand::C2Zone_USER_MOVE_FINISH:
				OnPacket_UpdateUserPosition(user, packet, true);
				break;
				//���� ���� ��(���Ͱ� �׾��ų� ���� ������ ������ ��)
			case RecvCommand::C2Zone_USER_ATTACK_FAILED:
				OnPacket_UserAttackFailed(user, packet);
				break;
				//���� ���� ��
			case RecvCommand::C2Zone_USER_ATTACK_MONSTER:
				OnPacket_UserAttack(user, packet);
				break;
				//���� ��Ȱ ��
			case RecvCommand::C2Zone_USER_REVIVE:
				OnPacket_UserRevive(user);
				break;
				//�ʵ� �̵�
			case RecvCommand::C2Zone_ENTER_FIELD:
				printf("[ Try To Enter Field ]\n");
				OnPacket_EnterField(user, packet);
				break;
				//�ʵ� �̵�
			case RecvCommand::C2Zone_ENTER_VILLAGE:
				printf("[ Try To Enter Village ]\n");
				OnPacket_EnterField(user, packet);
				break;
				//���� ���� ��
			case RecvCommand::C2Zone_EXIT_USER:
				OnPacket_UpdateUser(user, packet);
				user->Disconnect();
				break;
				//HeartBeatCheck 6ȸ �� ���� ������Ʈ ��û ��
			case RecvCommand::C2Zone_UPDATE_INFO:
				OnPacket_UpdateUser(user, packet);
				break;
			case RecvCommand::C2Zone_ENTER_TEST_USER:
				OnPacket_EnterTestUser(user, packet);
				break;
			}
		}
	}
}

void ServerLogicThread::ProcessMonsterPacket()
{
	while (1)
	{
		if (m_monsterPacketQueue.IsEmpty()) return;

		PacketQueuePair_Monster* PacketQueuePair_Monster = m_monsterPacketQueue.PopObject();
		Packet* packet = PacketQueuePair_Monster->packet;
		Monster* monster = PacketQueuePair_Monster->monster;

		//�ش� ���Ͱ� �׾��ִٸ� �н�
		if (monster->IsDeath()) continue;

		switch (static_cast<SendCommand>(packet->cmd))
		{
		case SendCommand::Zone2C_MONSTER_ATTACK:
			OnPacket_MonsterAttack(monster, packet);
			break;
		default:
			/*printf("=======================================\n");
			printf("Monster %d : ", monster->GetInfo().index);*/
			monster->GetField()->
				SectorSendAll(monster->GetSector()->GetRoundSectorsVec(), reinterpret_cast<char*>(packet), packet->size);
			break;
		}
	}
}

void ServerLogicThread::ProcessDBConnectorPacket()
{
	//if (!Connector->GetIsConnected())
	//{
	//	//������
	//	Connector->Connect();
	//}
	while (1)
	{
		if (m_dbPacketQueue.IsEmpty()) return;

		Packet* packet = m_dbPacketQueue.PopObject();

		switch (static_cast<RecvCommand>(packet->cmd))
		{
		case RecvCommand::DB2Zone_LOGIN_FAILED_INVALID_ID:
			OnPacket_LogInFailed(packet);
			break;
		case RecvCommand::DB2Zone_LOGIN_FAILED_WRONG_PASSWORD:
			OnPacket_LogInFailed(packet);
			break;
		case RecvCommand::DB2Zone_LOGIN_SUCCESS:
			OnPacket_LogInSuccess(packet);
			break;
		case RecvCommand::DB2Zone_REGISTER_FAILED:
			OnPacket_RegisterFailed(packet);
			break;
		case RecvCommand::DB2Zone_REGISTER_SUCCESS:
			OnPacket_RegisterSuccess(packet);
			break;
		case RecvCommand::DB2Zone_GET_USER_DATA_FAILED:
			OnPacket_GetUserInfoFailed(packet);
			break;
		case RecvCommand::DB2Zone_GET_USER_DATA_SUCCESS:
			OnPacket_GetUserInfoSuccess(packet);
			break;
		case RecvCommand::DB2Zone_MONSTERS_DATA:
			DBCONNECTOR->GetMonstersData(packet);
			m_fieldManager->InitMonsterThread();
			break;
		case RecvCommand::DB2Zone_UPDATE_USER_FAILED:
			OnPacket_UpdateUserFailed(packet);
			break;
		case RecvCommand::DB2Zone_UPDATE_USER_SUCCESS:
			OnPacket_UpdateUserSuccess(packet);
			break;
		}
	}
}

void ServerLogicThread::ConnectUser()
{
	while (1)
	{
		if (m_connectQueue.IsEmpty()) return;

		Session* tempSession = m_connectQueue.PopObject();

		m_sessionManager->AddSessionList(tempSession);
	}
}

void ServerLogicThread::DisConnectUser()
{
	while (1)
	{
		if (m_disconnectQueue.IsEmpty()) return;

		User* tempUser = dynamic_cast<User*>(m_disconnectQueue.PopObject());

		//���� �־��ٸ� �ش� ������ ���� ���� ��
		if (tempUser->GetField() != nullptr)
		{
			tempUser->GetField()->ExitUser(tempUser);
		}

		//���ǸŴ������� ������ �������ܰ� ���ÿ� ������ƮǮ�� ��ȯ���ش�.
		m_sessionManager->ReturnSessionList(tempUser);
		m_sessionManager->DeleteSessionID(tempUser->GetInfo()->userInfo.userID);
	}
}

void ServerLogicThread::AddToUserPacketQueue(PacketQueuePair_User* _userPacketQueuePair)
{
	m_userPacketQueue.AddObject(_userPacketQueuePair);

	SetEvent(m_hEvent[EVENT_RECV]);
}

void ServerLogicThread::AddToDBConnectorPacketQueue(Packet* _packet)
{
	m_dbPacketQueue.AddObject(_packet);

	SetEvent(m_hEvent[EVENT_DBCONNECTOR]);
}

void ServerLogicThread::AddToMonsterPacketQueue(PacketQueuePair_Monster* _monsterPacketQueuePair)
{
	m_monsterPacketQueue.AddObject(_monsterPacketQueuePair);

	SetEvent(m_hEvent[EVENT_MONSTER]);
}

void ServerLogicThread::AddToConnectQueue(Session* _session)
{
	m_connectQueue.AddObject(_session);

	SetEvent(m_hEvent[EVENT_CONNECT]);
}

void ServerLogicThread::AddToDisConnectQueue(Session* _session)
{
	m_disconnectQueue.AddObject(_session);

	SetEvent(m_hEvent[EVENT_DISCONNECT]);
}

void ServerLogicThread::OnPacket_EnterField(User* _user, Packet* _packet)
{
	FieldNumPacket* fieldNumPacket = static_cast<FieldNumPacket*>(_packet);

	Field* field = m_fieldManager->GetField(fieldNumPacket->fieldNum);
	if (field == nullptr) return;

	Field* prevField = m_fieldManager->GetField(_user->GetInfo()->unitInfo.fieldNum);
	if (prevField != nullptr)
	{
		printf("[ Exit User (Prev Field) ]");
		prevField->ExitUser(_user);
	}

	field->EnterUser(_user);
}

void ServerLogicThread::OnPacket_EnterFieldSuccess(User* _user)
{
	_user->StartCheckingHeartBeat();

	Field* field = _user->GetField();

	field->SendUserList(_user);
}

void ServerLogicThread::OnPacket_UpdateUserPosition(User* _user, Packet* _packet, bool _isFinish)
{
	UserPositionPacket* userPositionPacket = static_cast<UserPositionPacket*>(_packet);

	if (!_isFinish && _user->GetInfo()->unitInfo.state == STATE::IDLE) _user->SetState(STATE::MOVE);
	else if(_isFinish) _user->SetState(STATE::IDLE);

	Field* field = _user->GetField();

	_user->SetPosition(userPositionPacket->position);

	field->UpdateUserSector(_user);

	field->SectorSendAll(_user->GetSector()->GetRoundSectorsVec(), reinterpret_cast<char*>(_packet), _packet->size);
}

void ServerLogicThread::OnPacket_UserAttackFailed(User* _user, Packet* _packet)
{
	UserAttackPacket * userAttackPacket = static_cast<UserAttackPacket*>(_packet);

	Field* field = _user->GetField();
	field->SectorSendAll( _user->GetSector()->GetRoundSectorsVec(), reinterpret_cast<char*>(_packet), _packet->size);
}

void ServerLogicThread::OnPacket_UserAttack(User* _user, Packet* _packet)
{
	UserAttackPacket * userAttackPacket = static_cast<UserAttackPacket*>(_packet);

	Field* field = _user->GetField();

	if (field->UserAttack(_user, userAttackPacket->monsterIndex))
	{
		field->SectorSendAll(_user->GetSector()->GetRoundSectorsVec(), reinterpret_cast<char*>(_packet), _packet->size);
	}
}

void ServerLogicThread::OnPacket_UserRevive(User* _user)
{
	Field* field = _user->GetField();

	field->RespawnUser(_user);
}
 
void ServerLogicThread::OnPacket_MonsterAttack(Monster* _monster, Packet* _packet)
{
	MonsterAttackPacket * monsterAttackPacket = static_cast<MonsterAttackPacket*>(_packet);

	User* user = _monster->GetTarget();
	if (user == nullptr) return;

	user->Hit(monsterAttackPacket->monsterIndex, monsterAttackPacket->damage);
}

void ServerLogicThread::OnPacket_RegisterUser(User* _user, Packet* _packet)
{
	RegisterUserPacket* registerUserPacket = static_cast<RegisterUserPacket*>(_packet);

	_user->RegisterUser(registerUserPacket);
}

void ServerLogicThread::OnPacket_LogInUser(User* _user, Packet* _packet)
{
	LogInPacket* logInPacket = static_cast<LogInPacket*>(_packet);

	_user->LogInUser(logInPacket);
}

void ServerLogicThread::OnPacket_UpdateUser(User* _user, Packet* _packet)
{
	SessionInfoPacket* sessionInfoPacket = static_cast<SessionInfoPacket*>(_packet);

	_user->GetInfo()->unitInfo = sessionInfoPacket->info.unitInfo;
	_user->GetInfo()->userInfo = sessionInfoPacket->info.userInfo;

	_user->UpdateInfo();
}

void ServerLogicThread::OnPacket_LogInSuccess(Packet* _packet)
{
	LogInSuccessPacket* logInSuccessPacket = static_cast<LogInSuccessPacket*>(_packet);
	
	User* tempUser;
	tempUser = dynamic_cast<User*>(FindUser(logInSuccessPacket->socket));

	if (tempUser != nullptr)
	{
		//�ߺ� �α���üũ
		if (m_sessionManager->FindSessionID(tempUser->GetInfo()->userInfo.userID))
		{
			//�ߺ� �α����̴� ������������
			tempUser->LogInDuplicated();

			return;
		}

		m_sessionManager->AddSessionID(tempUser->GetInfo()->userInfo.userID);

		tempUser->LogInSuccess(logInSuccessPacket->userIndex);
	}
}

void ServerLogicThread::OnPacket_LogInFailed(Packet* _packet)
{
	PacketWithSocket* packetWithSocket = static_cast<PacketWithSocket*>(_packet);

	User* tempUser;
	tempUser = dynamic_cast<User*>(FindUser(packetWithSocket->socket));

	if (tempUser != nullptr)
	{
		tempUser->LogInFailed();
	}
}

void ServerLogicThread::OnPacket_RegisterSuccess(Packet* _packet)
{
	PacketWithSocket* packetWithSocket = static_cast<PacketWithSocket*>(_packet);

	User* tempUser;
	tempUser = dynamic_cast<User*>(FindUser(packetWithSocket->socket));

	if (tempUser != nullptr)
	{
		tempUser->RegisterSuccess();
	}
}

void ServerLogicThread::OnPacket_RegisterFailed(Packet* _packet)
{
	PacketWithSocket* packetWithSocket = static_cast<PacketWithSocket*>(_packet);

	User* tempUser;
	tempUser = dynamic_cast<User*>(FindUser(packetWithSocket->socket));

	if (tempUser != nullptr)
	{
		tempUser->RegisterFailed();
	}
}

void ServerLogicThread::OnPacket_GetUserInfoSuccess(Packet* _packet)
{
	GetSessionInfoPacket* getSessionInfoPacket = static_cast<GetSessionInfoPacket*>(_packet);

	User* tempUser;
	tempUser = dynamic_cast<User*>(FindUser(getSessionInfoPacket->socket));

	if (tempUser != nullptr)
	{
		tempUser->SendInfo(getSessionInfoPacket);
	}
}

void ServerLogicThread::OnPacket_GetUserInfoFailed(Packet* _packet)
{
	PacketWithSocket* packetWithSocket = static_cast<PacketWithSocket*>(_packet);

	User* tempUser;
	tempUser = dynamic_cast<User*>(FindUser(packetWithSocket->socket));

	if (tempUser != nullptr)
	{
		tempUser->RequestUserInfoFailed();
	}
}

void ServerLogicThread::OnPacket_UpdateUserSuccess(Packet* _packet)
{
	PacketWithSocket* packetWithSocket = static_cast<PacketWithSocket*>(_packet);

	User* tempUser;
	tempUser = dynamic_cast<User*>(FindUser(packetWithSocket->socket));

	if (tempUser != nullptr)
	{
		printf("[ %d user - INFO UPDATE TO DATABASE SUCCESS ] \n", tempUser->GetInfo()->userInfo.userID);
	}
}

void ServerLogicThread::OnPacket_UpdateUserFailed(Packet* _packet)
{
	PacketWithSocket* packetWithSocket = static_cast<PacketWithSocket*>(_packet);

	User* tempUser;
	tempUser = dynamic_cast<User*>(FindUser(packetWithSocket->socket));

	if (tempUser != nullptr)
	{
		//��õ�
		tempUser->UpdateInfo();
	}
}

Session* ServerLogicThread::FindUser(SOCKET _socket)
{
	list<Session*>* vSessionList = m_sessionManager->GetItemList();

	for (const auto& element : *vSessionList)
	{
		if (element->GetSocket() == _socket)
		{
			return element;
		}
	}

	return nullptr;
}

void ServerLogicThread::OnPacket_EnterTestUser(User* _user, Packet* _packet)
{
	TestClientEnterPacket* testClientEnterPacket = static_cast<TestClientEnterPacket*>(_packet);

	Field* field = m_fieldManager->GetField(testClientEnterPacket->fieldNum);

	field->EnterTestClient(_user, testClientEnterPacket->userNum);

	//_user->StartCheckingHeartBeat();
}