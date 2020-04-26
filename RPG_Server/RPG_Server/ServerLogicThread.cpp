#include "ServerLogicThread.h"

ServerLogicThread::ServerLogicThread()
{
}

ServerLogicThread::~ServerLogicThread()
{
}

void ServerLogicThread::Init(SessionManager* _sessionManager,
							FieldManager* _fieldManager)
{
	m_sessionManager = _sessionManager;
	m_fieldManager = _fieldManager;

	ThreadClass<ServerLogicThread>::Start(this);

	printf("[ ServerLogicThread Init Success ]\n");
}

void ServerLogicThread::LoopRun()
{
	while(1)
	{
		//start = clock();

		ParsingUser();

		//end = clock();

		//double result = (double)(end - start);
		//printf("1 : %.3f \n", result / 1000);

		ParsingMonster();

		Sleep(1);
	}
}

void ServerLogicThread::ParsingUser()
{
	list<Session*>* vSessionList = m_sessionManager->GetItemList();

	/*list<Session*>::iterator iterBegin = vSessionList->begin();
	list<Session*>::iterator iterEnd = vSessionList->end();

	User* user;

	
	for_each(iterBegin, iterEnd, [=](Session* _session) mutable
	{
		user = dynamic_cast<User*>(_session);

		if (!user->GetIsConnected())
		{
			//���� �־��ٸ� �ش� ������ ���� ���� ��
			if (user->GetZone() != nullptr)
			{
				user->GetZone()->ExitUser(user);
			}

			//shutdown ���� close
			closesocket(user->GetSocket());

			//���ǸŴ������� ������ �������ܰ� ���ÿ� ������ƮǮ�� ��ȯ���ش�.
			m_sessionManager->DeleteSession(user);
			m_sessionManager->DeleteSessionID(user->GetInfo()->userInfo.userID);

			printf("[ ������ �� : %d ] \n", m_sessionManager->GetItemList()->size());
		}
		else
		{
			Packet* packet = reinterpret_cast<Packet*>(user->GetRecvBuffer()->CanParsing());

			if (packet != nullptr)
			{
				//�׽�Ʈ�� ��Ŷ ó��
				if (packet->cmd == 1234)
				{
					ZoneNumPacket* zoneNumPacket = static_cast<ZoneNumPacket*>(packet);

					Zone* zone = m_zoneManager->GetZone(zoneNumPacket->zoneNum);

					zone->EnterTestClient(user);
				}
				else if (packet->cmd == 12345)
				{
					printf("Test Checking \n");
				}
				else
				{
					//�Ϲ����� ������ ��Ŷ ó�� ����ġ��
					switch (static_cast<RecvCommand>(packet->cmd))
					{
						//ä�� ��Ŷ
						//�ٷ� �ش� ���� SendAll
					case RecvCommand::CHATTING:
						user->GetZone()->SendAll(reinterpret_cast<char*>(packet), packet->size);
						break;
						//HeartBeatChecked ���� ��Ŷ ���� ��
					case RecvCommand::CHECK_ALIVE:
						user->SetIsChecking(false);
						user->HeartBeatChecked();
						break;
						//ȸ������ �õ���
					case RecvCommand::REGISTER_ACCOUNT:
						RegisterUser(user, packet);
						break;
						//�α��� �õ���
					case RecvCommand::LOGIN:
						LogInUser(user, packet);
						break;
						//���� �� ���� ���� ��û ��
					case RecvCommand::REQUIRE_INFO:
						user->SendInfo();
						break;
						//���� ���� �� ù �� ���� �� �޴� ��Ŷ
						//���� ��ġ, Ÿ�� ����, ���� �־��ٸ� �ش� �������� Exitó��
						//�ش� ���� ���� ���� �����鿡�� �����ߴٰ� �˷���
					case RecvCommand::TRY_ENTER_ZONE:
						EnterZone(user, packet);
						break;
						//���� ���� ������ ����Ʈ ������
					case RecvCommand::ENTER_ZONE_SUCCESS:
						EnterZoneSuccess(user);
						break;
						//���� �̵�
					case RecvCommand::USER_MOVE:
						UpdateUserPosition(user, packet);
						break;
						//���� �̵�
					case RecvCommand::USER_MOVE_FINISH:
						UpdateUserPosition(user, packet);
						break;
						//���� ���� ��(���Ͱ� �׾��ų� ���� ������ ������ ��)
					case RecvCommand::USER_ATTACK_FAILED:
						UserAttackFailed(user, packet);
						break;
						//���� ���� ��
					case RecvCommand::USER_ATTACK_MONSTER:
						UserAttack(user, packet);
						break;
						//���� ��Ȱ ��
					case RecvCommand::USER_REVIVE:
						UserRevive(user);
						break;
						//
					case RecvCommand::ENTER_FIELD_ZONE:
						printf("[ Try To Enter Field Zone ]\n");
						EnterZone(user, packet);
						break;
						//
					case RecvCommand::ENTER_VILLAGE_ZONE:
						printf("[ Try To Enter Village Zone ]\n");
						EnterZone(user, packet);
						break;
						//���� ���� ��
					case RecvCommand::EXIT_USER:
						UpdateUser(user, packet);
						user->Disconnect();
						break;
						//HeartBeatCheck 6ȸ �� ���� ������Ʈ ��û ��
					case RecvCommand::UPDATE_INFO:
						UpdateUser(user, packet);
						break;
					}
				}
			}
		}
	});
	*/

	//�Ϲ����� For��
	list<Session*>::iterator iterEnd = vSessionList->end();

	User* user;

	for (list<Session*>::iterator iter = vSessionList->begin(); iter != iterEnd;)
	{
		user = dynamic_cast<User*>(*iter);
		++iter;

		if (!user->GetIsConnected())
		{
			//���� �־��ٸ� �ش� ������ ���� ���� ��
			if (user->GetField() != nullptr)
			{
				user->GetField()->ExitUser(user);
			}

			//shutdown ���� close
			closesocket(user->GetSocket());

			//���ǸŴ������� ������ �������ܰ� ���ÿ� ������ƮǮ�� ��ȯ���ش�.
			m_sessionManager->DeleteSession(user);
			m_sessionManager->DeleteSessionID(user->GetInfo()->userInfo.userID);

			printf("[ ������ �� : %d ] \n", m_sessionManager->GetItemList()->size());

			continue;
		}

		Packet* packet = reinterpret_cast<Packet*>(user->GetRecvBuffer()->CanParsing());

		if (packet != nullptr)
		{
			//�׽�Ʈ�� ��Ŷ ó��
			if (packet->cmd == 1234)
			{
				FieldNumPacket* fieldNumPacket = static_cast<FieldNumPacket*>(packet);

				Field* field = m_fieldManager->GetField(fieldNumPacket->fieldNum);

				field->EnterTestClient(user);
			}
			else if (packet->cmd == 12345)
			{
				printf("Test Checking \n");
			}
			else
			{
				//�Ϲ����� ������ ��Ŷ ó�� ����ġ��
				switch (static_cast<RecvCommand>(packet->cmd))
				{
					//ä�� ��Ŷ
					//�ٷ� �ش� ���� SendAll
				case RecvCommand::C2Zone_CHATTING:
					user->GetField()->
						SectorSendAll(user->GetSector()->GetRoundSectorsVec(), reinterpret_cast<char*>(packet), packet->size);
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
					//�̷� �������� ��Ŷ �Լ����� �������.
					OnPacket_LogInUser(user, packet);
					break;
					//���� �� ���� ���� ��û ��
				case RecvCommand::C2Zone_REQUIRE_INFO:
					user->SendInfo();
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
					OnPacket_UpdateUserPosition(user, packet);
					break;
					//���� �̵�
				case RecvCommand::C2Zone_USER_MOVE_FINISH:
					OnPacket_UpdateUserPosition(user, packet);
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
					//
				case RecvCommand::C2Zone_ENTER_FIELD:
					printf("[ Try To Enter Field ]\n");
					OnPacket_EnterField(user, packet);
					break;
					//
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
				}
			}
		}
	}
}

void ServerLogicThread::ParsingMonster()
{
	while (1)
	{
		if (m_monsterPacketQueue.IsEmpty()) return;

		PacketQueuePair* packetQueuePair = &m_monsterPacketQueue.GetItem();
		Packet* packet = packetQueuePair->packet;
		Monster* monster = packetQueuePair->monster;

		//�ش� ���Ͱ� �׾��ִٸ� �н�
		if (monster->GetIsDeath()) continue;

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

void ServerLogicThread::OnPacket_UpdateUserPosition(User* _user, Packet* _packet)
{
	UserPositionPacket* userPositionPacket = static_cast<UserPositionPacket*>(_packet);

	Field* field = _user->GetField();

	_user->SetPosition(userPositionPacket->position);

	field->UpdateUserSector(_user);

	//field->FieldSendAll(reinterpret_cast<char*>(_packet), _packet->size);

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

	if (_user->LogInUser(logInPacket))
	{
		//�ߺ� �α���üũ
		if (m_sessionManager->FindSessionID(_user->GetInfo()->userInfo.userID))
		{
			//�ߺ� �α����̴� ������������
			_user->LogInDuplicated();

			return;
		}

		m_sessionManager->AddSessionID(_user->GetInfo()->userInfo.userID);

		_user->LogInSuccess();
	}
}

void ServerLogicThread::OnPacket_UpdateUser(User* _user, Packet* _packet)
{
	SessionInfoPacket* sessionInfoPacket = static_cast<SessionInfoPacket*>(_packet);

	_user->GetInfo()->unitInfo = sessionInfoPacket->info.unitInfo;
	_user->GetInfo()->userInfo = sessionInfoPacket->info.userInfo;

	_user->UpdateInfo();
}