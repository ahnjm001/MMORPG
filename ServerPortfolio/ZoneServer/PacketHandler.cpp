#include "PacketHandler.h"

#include "DBConnector.h"

PacketHandler::~PacketHandler()
{
}

void PacketHandler::HandleUserPacket(User* _user, Packet* _packet)
{
	Packet* packet = _packet;
	User* user = _user;

	(this->*userPacketFunc[packet->cmd])(user, packet);

	user->GetReceiver()->GetRingBuffer()->Read(packet->size);
}

void PacketHandler::HandleMonsterPacket(Monster* _monster, Packet* _packet)
{
	Packet* packet = _packet;
	Monster* monster = _monster;

	switch (static_cast<SendCommand>(packet->cmd))
	{
	case SendCommand::Zone2C_MONSTER_ATTACK:
		OnPacket_MonsterAttack(monster, packet);
		break;
	default:
		monster->GetField()->
			SectorSendAll(monster->GetSector()->GetRoundSectorsVec(), reinterpret_cast<char*>(packet), packet->size);
		break;
	}
}

void PacketHandler::HandleDBConnectorPacket(Packet* _packet)
{
	Packet* packet = _packet;

	(this->*dbPacketFunc[packet->cmd % 100])(packet);

	DBCONNECTOR->GetReceiver()->GetRingBuffer()->Read(packet->size);
}

void PacketHandler::OnPacket_EnterField(User* _user, Packet* _packet)
{
	FieldNumPacket* fieldNumPacket = reinterpret_cast<FieldNumPacket*>(_packet);

	Field* field = m_fieldManager.GetField(fieldNumPacket->fieldNum);
	if (field == nullptr) return;

	Field* prevField = m_fieldManager.GetField(_user->GetInfo()->unitInfo.fieldNum);
	if (prevField != nullptr)
	{
		MYDEBUG("[ Exit User (Prev Field) ]");
		prevField->ExitUser(_user);
	}

	field->EnterUser(_user);
}

void PacketHandler::OnPacket_EnterFieldSuccess(User* _user, Packet* _packet)
{
	Field* field = _user->GetField();

	field->SendUserList(_user);
}

void PacketHandler::OnPacket_UpdateUserPosition(User* _user, Packet* _packet)
{
	UserPositionPacket* userPositionPacket = reinterpret_cast<UserPositionPacket*>(_packet);

	if (_user->GetInfo()->unitInfo.state == STATE::IDLE)
	{
		_user->SetState(STATE::MOVE);
	}

	Field* field = _user->GetField();

	_user->SetPosition(userPositionPacket->position);

	field->UpdateUserSector(_user);

	field->SectorSendAll(_user->GetSector()->GetRoundSectorsVec(), reinterpret_cast<char*>(_packet), _packet->size);
}

void PacketHandler::OnPacket_UpdateUserPosition_Finish(User* _user, Packet* _packet)
{
	UserPositionPacket* userPositionPacket = reinterpret_cast<UserPositionPacket*>(_packet);

	_user->SetState(STATE::IDLE);

	Field* field = _user->GetField();

	_user->SetPosition(userPositionPacket->position);

	field->UpdateUserSector(_user);

	field->SectorSendAll(_user->GetSector()->GetRoundSectorsVec(), reinterpret_cast<char*>(_packet), _packet->size);
}

void PacketHandler::OnPacket_UserAttackFailed(User* _user, Packet* _packet)
{
	UserAttackPacket * userAttackPacket = reinterpret_cast<UserAttackPacket*>(_packet);

	Field* field = _user->GetField();
	field->SectorSendAll(_user->GetSector()->GetRoundSectorsVec(), reinterpret_cast<char*>(_packet), _packet->size);
}

void PacketHandler::OnPacket_UserAttack(User* _user, Packet* _packet)
{
	UserAttackPacket * userAttackPacket = reinterpret_cast<UserAttackPacket*>(_packet);

	Field* field = _user->GetField();

	if (field->UserAttack(_user, userAttackPacket->monsterIndex))
	{
		field->SectorSendAll(_user->GetSector()->GetRoundSectorsVec(), reinterpret_cast<char*>(_packet), _packet->size);
	}
}

void PacketHandler::OnPacket_UserRevive(User* _user, Packet* _packet)
{
	Field* field = _user->GetField();

	field->RespawnUser(_user);
}

void PacketHandler::OnPacket_MonsterAttack(Monster* _monster, Packet* _packet)
{
	MonsterAttackPacket * monsterAttackPacket = reinterpret_cast<MonsterAttackPacket*>(_packet);

	User* user = _monster->GetTarget();
	if (user == nullptr) return;

	user->Hit(monsterAttackPacket->monsterIndex, monsterAttackPacket->damage);
}

void PacketHandler::OnPacket_RequireInfo(User* _user, Packet* _packet)
{
	LogInSuccessPacket* logInSuccessPacket = reinterpret_cast<LogInSuccessPacket*>(_packet);

	//�ߺ� �α���üũ(���߿� DB���� ���ν����� �̿��ؼ� ����)
	if (m_userManager.GetUserHashMap()->IsExist(_user->GetInfo()->userInfo.userID))
	{
		//�ߺ� �α����̴� ������������
		_user->LogInDuplicated();

		return;
	}

	_user->RequestUserInfo(logInSuccessPacket->userIndex);

	MainThread::getSingleton()->AddToHashMapQueue(_user);
}

void PacketHandler::OnPacket_UpdateUser(User* _user, Packet* _packet)
{
	SessionInfoPacket* sessionInfoPacket = reinterpret_cast<SessionInfoPacket*>(_packet);

	_user->GetInfo()->unitInfo = sessionInfoPacket->info.unitInfo;
	_user->GetInfo()->userInfo = sessionInfoPacket->info.userInfo;

	_user->UpdateInfo();
}

void PacketHandler::OnPacket_ExitUser(User* _user, Packet* _packet)
{
	OnPacket_UpdateUser(_user, _packet);

	_user->DisConnect();
}

void PacketHandler::OnPacket_Chatting(User* _user, Packet* _packet)
{
	ChattingPacket* chattingPacket = reinterpret_cast<ChattingPacket*>(_packet);

	//MYDEBUG("%s \n", chattingPacket->id);

	_user->GetField()->
		SectorSendAll(_user->GetSector()->GetRoundSectorsVec(),
			reinterpret_cast<char*>(_packet), _packet->size);
}

void PacketHandler::OnPacket_HeartBeat(User* _user, Packet* _packet)
{
	_user->HeartBeatChecked();
}

void PacketHandler::OnPacket_GetUserInfoSuccess(Packet* _packet)
{
	GetSessionInfoPacket* getSessionInfoPacket = reinterpret_cast<GetSessionInfoPacket*>(_packet);

	User* tempUser;
	tempUser = FindUserAtHashMap(getSessionInfoPacket->socket);

	if (tempUser != nullptr)
	{
		tempUser->SendInfo(getSessionInfoPacket);
	}
}

void PacketHandler::OnPacket_GetUserInfoFailed(Packet* _packet)
{
	PacketWithSocket* packetWithSocket = reinterpret_cast<PacketWithSocket*>(_packet);

	User* tempUser;
	tempUser = FindUserAtHashMap(packetWithSocket->socket);

	if (tempUser != nullptr)
	{
		tempUser->RequestUserInfoFailed();
	}
}

void PacketHandler::OnPacket_UpdateUserSuccess(Packet* _packet)
{
	PacketWithSocket* packetWithSocket = reinterpret_cast<PacketWithSocket*>(_packet);

	User* tempUser;
	tempUser = FindUserAtHashMap(packetWithSocket->socket);

	if (tempUser != nullptr)
	{
		MYDEBUG("[ %d user - INFO UPDATE TO DATABASE SUCCESS ] \n", tempUser->GetInfo()->userInfo.userID);
	}
}

void PacketHandler::OnPacket_UpdateUserFailed(Packet* _packet)
{
	PacketWithSocket* packetWithSocket = reinterpret_cast<PacketWithSocket*>(_packet);

	User* tempUser;
	tempUser = FindUserAtHashMap(packetWithSocket->socket);

	if (tempUser != nullptr)
	{
		//��õ�
		tempUser->UpdateInfo();
	}
}

void PacketHandler::OnPakcet_GetMonstersData(Packet* _packet)
{
	DBCONNECTOR->GetMonstersData(_packet);

	m_fieldManager.InitMonsterThread();
}

User* PacketHandler::FindUserAtHashMap(SOCKET _socket)
{
	const std::unordered_map<int, User*> tempHashMap = m_userManager.GetUserHashMap()->GetItemHashMap();

	for (const auto& element : tempHashMap)
	{
		if (element.second->GetSocket() == _socket)
		{
			return element.second;
		}
	}

	return nullptr;
}

void PacketHandler::OnPacket_EnterTestUser(User* _user, Packet* _packet)
{
	TestClientEnterPacket* testClientEnterPacket = reinterpret_cast<TestClientEnterPacket*>(_packet);

	Field* field = m_fieldManager.GetField(testClientEnterPacket->fieldNum);
	if (field == nullptr) return;

	field->EnterTestClient(_user, testClientEnterPacket->userNum);
}

void PacketHandler::OnPacket_Chatting_Whisper(User* _user, Packet* _packet)
{
	ChattingPacket_Whisper* whisperChattingPacket = reinterpret_cast<ChattingPacket_Whisper*>(_packet);

	//How to search by value in a Map
	//https://thispointer.com/how-to-search-by-value-in-a-map-c/
	//�ٸ� ���� ��� ����?
	const std::unordered_map<int, User*> tempHashMap = m_userManager.GetUserHashMap()->GetItemHashMap();
	User* tempUser;

	auto iter = tempHashMap.begin();

	while (iter != tempHashMap.end())
	{
		tempUser = iter->second;

		if (strcmp(tempUser->GetInfo()->userInfo.userName, whisperChattingPacket->targetId) == 0)
		{
			_user->Send(reinterpret_cast<char*>(_packet), _packet->size);
			tempUser->Send(reinterpret_cast<char*>(_packet), _packet->size);

			return;
		}

		iter++;
	}

	Packet* whisperFailpacket =
		reinterpret_cast<Packet*>(_user->GetSendBuffer()->
			GetBuffer(sizeof(Packet)));
	whisperFailpacket->Init(SendCommand::Zone2C_CHATTING_WHISPER_FAIL, sizeof(Packet));

	_user->Send(reinterpret_cast<char*>(whisperFailpacket), whisperFailpacket->size);
}