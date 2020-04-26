#include "Field.h"

Field::Field()
{
}

Field::~Field()
{
}

void Field::Init(int _num, VECTOR2 _spawnPosition)
{
	m_fieldNum = _num;
	m_spawnPosition = _spawnPosition;

	m_sendBuffer = new SendBuffer();
	m_sendBuffer->Init(10000);

	m_leaveSectorsVec.resize(9);
	m_enterSectorsVec.resize(9);

	printf("[ %d Field Init ,", _num);
}

void Field::GetMap(const char* _name)
{
	m_fieldTilesData.GetMap(_name);

	m_sectorManager.Init(&m_fieldTilesData);
	
	m_monsterLogicThread.Init(this, &m_fieldTilesData, &m_sectorManager);
	m_monsterLogicThread.ThreadClass<MonsterLogicThread>::Start(&m_monsterLogicThread);
}

void Field::FieldSendAll(char * _buffer, int _size)
{
	for (const auto& element : m_itemList)
	{
		if (!element->GetIsTestClient())
		{
			element->Send(_buffer, _size);
		}
	}
}

void Field::SectorSendAll(std::vector<Sector*>* _sectorsVec, char * _buffer, int _size)
{
	std::vector<Sector*> tempVector = *_sectorsVec;

	for (const auto& element_1 : tempVector)
	{
		if (element_1 == nullptr) continue;

		std::list<User*> tempList = *element_1->Manager<User>::GetItemList();

		if (tempList.size() <= 0) continue;

		for (const auto& element_2 : tempList)
		{
			if (!element_2->GetIsTestClient())
			{
				element_2->Send(_buffer, _size);
			}
		}
	}
}

void Field::SendUserList(User* _user)
{
	UserListPacket* userListPacket =
		reinterpret_cast<UserListPacket*>(_user->GetSendBuffer()->
			GetBuffer(sizeof(UserListPacket)));

	userListPacket->userNum = 0;

	int i = 0;

	for (const auto& element : m_itemList)
	{
		if (element->GetIsTestClient()) continue;

		userListPacket->info[i].userInfo = element->GetInfo()->userInfo;
		userListPacket->info[i].unitInfo = element->GetInfo()->unitInfo;

		i++;
		userListPacket->userNum++;
	}

	userListPacket->size = (sizeof(BasicInfo) * userListPacket->userNum)
							+ sizeof(WORD) + sizeof(Packet);
	userListPacket->Init(SendCommand::Zone2C_USER_LIST, userListPacket->size);

	_user->GetSendBuffer()->Write(userListPacket->size);
	_user->Send(reinterpret_cast<char*>(userListPacket), userListPacket->size);

	SendUserList_InRange(_user);

	printf("[ USER LIST 전송 완료 ]\n");

	m_monsterLogicThread.SendMonsterList(_user);
}

void Field::SendUserList_InRange(User* _user)
{
	int tempNum = 0;

	UserListPacket* userListPacket_InRange =
		reinterpret_cast<UserListPacket*>(_user->GetSendBuffer()->
			GetBuffer(sizeof(UserListPacket)));

	userListPacket_InRange->userNum = 0;

	std::vector<Sector*> tempVec = *_user->GetSector()->GetRoundSectorsVec();

	for (const auto& tempSector : tempVec)
	{
		std::list<User*> tempList = *tempSector->Manager<User>::GetItemList();

		if (tempList.size() <= 0) continue;

		for (const auto& tempUser : tempList)
		{
			if (tempUser->GetIsTestClient()) continue;

			userListPacket_InRange->info[tempNum].userInfo = tempUser->GetInfo()->userInfo;
			userListPacket_InRange->info[tempNum].unitInfo = tempUser->GetInfo()->unitInfo;

			tempNum++;
			userListPacket_InRange->userNum++;
		}
	}

	/*for (int i = 0; i < 9; i++)
	{
		Sector* tempSector = _user->GetSector()->GetRoundSectors()[i];
		std::list<User*> tempList = *tempSector->Manager<User>::GetItemList();

		if (tempSector == nullptr) continue;

		if (tempList.size() <= 0) continue;

		for (const auto& element : tempList)
		{
			if (element->GetIsTestClient()) continue;

			userListPacket_InRange->info[tempNum].userInfo = element->GetInfo()->userInfo;
			userListPacket_InRange->info[tempNum].unitInfo = element->GetInfo()->unitInfo;

			tempNum++;
			userListPacket_InRange->userNum++;
		}
	}*/

	userListPacket_InRange->size = (sizeof(BasicInfo) * userListPacket_InRange->userNum)
		+ sizeof(WORD) + sizeof(Packet);
	userListPacket_InRange->Init(SendCommand::Zone2C_USER_LIST_IN_RANGE, userListPacket_InRange->size);

	_user->GetSendBuffer()->Write(userListPacket_InRange->size);
	_user->Send(reinterpret_cast<char*>(userListPacket_InRange), userListPacket_InRange->size);
}

//테스트용
void Field::EnterTestClient(User* _user)
{
	_user->TestClientEnterField(this, m_fieldNum);

	m_locker.EnterLock();

	AddItem(_user);

	m_locker.LeaveLock();

	printf("[%d Field : Test Client Insert - %d (", m_fieldNum, _user->GetInfo()->userInfo.userID);
	printf("user count : %d) ]\n", (int)m_itemList.size());
}

void Field::EnterUser(User* _user)
{
	if (_user->GetField() != nullptr)
	{
		if (_user->GetField()->GetFieldNum() == 2)
		{	
			VECTOR2 tempVec;
			tempVec.x = 52.0f;
			tempVec.y = 86.0f;

			_user->EnterField(this, m_fieldNum, tempVec);
		}
		else
		{
			_user->EnterField(this, m_fieldNum, m_spawnPosition);
		}
	}
	else
	{
		_user->EnterField(this, m_fieldNum, m_spawnPosition);
	}

	m_locker.EnterLock();

	AddItem(_user);

	UpdateUserSector(_user);

	SendEnterUserInfo(_user);

	m_locker.LeaveLock();

	printf("[%d Field : User Insert - %d (", m_fieldNum, _user->GetInfo()->userInfo.userID);
	printf("user count : %d) ]\n", (int)m_itemList.size());
}

void Field::SendEnterUserInfo(User* _user)
{
	SessionInfoPacket* sessionInfoPacket =
		reinterpret_cast<SessionInfoPacket*>(m_sendBuffer->
			GetBuffer(sizeof(SessionInfoPacket)));
	sessionInfoPacket->Init(SendCommand::Zone2C_ENTER_USER_INFO, sizeof(SessionInfoPacket));
	sessionInfoPacket->info.userInfo = _user->GetInfo()->userInfo;
	sessionInfoPacket->info.unitInfo = _user->GetInfo()->unitInfo;
	m_sendBuffer->Write(sessionInfoPacket->size);

	for (const auto& element : m_itemList)
	{
		if (element->GetIsTestClient()) continue;

		if (element->GetInfo()->userInfo.userID ==
			sessionInfoPacket->info.userInfo.userID) continue;

		element->Send(reinterpret_cast<char*>(sessionInfoPacket), sessionInfoPacket->size);
	}

	UserNumPacket* userNumPacket_InRange =
		reinterpret_cast<UserNumPacket*>(m_sendBuffer->
			GetBuffer(sizeof(UserNumPacket)));
	userNumPacket_InRange->Init(SendCommand::Zone2C_ENTER_USER_INFO_IN_RANGE, sizeof(UserNumPacket));
	userNumPacket_InRange->userIndex = _user->GetInfo()->userInfo.userID;
	m_sendBuffer->Write(userNumPacket_InRange->size);

	std::vector<Sector*> tempVec = *_user->GetSector()->GetRoundSectorsVec();

	for (const auto& tempSector : tempVec)
	{
		std::list<User*> tempList = *tempSector->Manager<User>::GetItemList();

		if (tempList.size() <= 0) continue;

		for (const auto& tempUser : tempList)
		{
			if (tempUser->GetIsTestClient()) continue;

			if (tempUser->GetInfo()->userInfo.userID ==
				userNumPacket_InRange->userIndex) continue;

			tempUser->Send(reinterpret_cast<char*>(userNumPacket_InRange), userNumPacket_InRange->size);
		}
	}

	/*for (int i = 0; i < 9; i++)
	{
		Sector* tempSector = _user->GetSector()->GetRoundSectors()[i];
		std::list<User*> tempList = *tempSector->Manager<User>::GetItemList();

		if (tempSector == nullptr) continue;

		if (tempList.size() <= 0) continue;

		for (const auto& element : tempList)
		{
			if (element == nullptr) break;

			if (element->GetIsTestClient()) continue;

			if (element->GetInfo()->userInfo.userID ==
				userNumPacket_InRange->userIndex) continue;

			element->Send(reinterpret_cast<char*>(userNumPacket_InRange), userNumPacket_InRange->size);
		}
	}*/
}

void Field::ExitUser(User* _user)
{
	m_locker.EnterLock();

	DeleteItem(_user);

	_user->GetSector()->Manager<User>::DeleteItem(_user);

	SendExitUserInfo(_user->GetInfo()->userInfo.userID);

	m_locker.LeaveLock();

	printf("[%d Field] : User Delete - %d (", m_fieldNum, _user->GetInfo()->userInfo.userID);
	printf("user count : %d)\n", (int)m_itemList.size());
}

void Field::SendExitUserInfo(int _num)
{
	UserNumPacket* userPacket =
		reinterpret_cast<UserNumPacket*>(m_sendBuffer->
			GetBuffer(sizeof(UserNumPacket)));
	userPacket->Init(SendCommand::Zone2C_LEAVE_USER_INFO, sizeof(UserNumPacket));
	userPacket->userIndex = _num;
	m_sendBuffer->Write(userPacket->size);

	FieldSendAll(reinterpret_cast<char*>(userPacket), userPacket->size);
}

bool Field::UserAttack(User * _user, int _monsterIndex)
{
	Monster* monster = m_monsterLogicThread.GetMonster(_monsterIndex);

	if (monster == nullptr) return false;

	Packet* packet = monster->Hit(_user, _user->GetInfo()->unitInfo.atk);
	if (packet == nullptr) return false;

	SectorSendAll(_user->GetSector()->GetRoundSectorsVec(), reinterpret_cast<char*>(packet), packet->size);

	if (monster->GetIsDeath())
	{
		_user->PlusExp(monster->GetData().dropExp);
	}

	return true;
}

void Field::RespawnUser(User* _user)
{
	_user->Respawn(m_spawnPosition);

	UpdateUserSector(_user);
}

void Field::UpdateUserSector(User* _user)
{
	Sector* nowSector = m_sectorManager.GetSector(_user->GetTile()->GetX(), 
		_user->GetTile()->GetY());

	//true == same, false = different
	if (_user->CompareSector(nowSector))
	{
		//true
		return;
	}

	//false
	//다르니까 섹터 변경 및 전 섹터가 유저가 나간 것을 반영
	Sector* prevSector = _user->GetSector();

	if (prevSector != nullptr)
	{
		printf("[ Exit User (Prev Sector) ] %s User : Now Sector : %d\n",
			_user->GetInfo()->userInfo.userName, _user->GetSector()->GetSectorNum());
		prevSector->Manager<User>::DeleteItem(_user);

		_user->SetSector(nowSector);
		_user->GetSector()->Manager<User>::AddItem(_user);

		m_locker.EnterLock();

		std::vector<Sector*> prevSectorsNeighbor = *prevSector->GetRoundSectorsVec();
		std::vector<Sector*> nowSectorsNeighbor = *nowSector->GetRoundSectorsVec();

		std::set_difference(
			prevSectorsNeighbor.begin(), prevSectorsNeighbor.end(),
			nowSectorsNeighbor.begin(), nowSectorsNeighbor.end(),
			m_leaveSectorsVec.begin());

		std::set_difference(
			nowSectorsNeighbor.begin(), nowSectorsNeighbor.end(),
			prevSectorsNeighbor.begin(), prevSectorsNeighbor.end(),
			m_enterSectorsVec.begin());

		LeaveSector(_user);

		EnterSector(_user);

		m_locker.LeaveLock();

		return;
	}

	_user->SetSector(nowSector);
	_user->GetSector()->Manager<User>::AddItem(_user);

	return;
}

void Field::LeaveSector(User* _user)
{
	UserNumPacket* userNumPacket_Exit =
		reinterpret_cast<UserNumPacket*>(m_sendBuffer->
			GetBuffer(sizeof(UserNumPacket)));
	userNumPacket_Exit->Init(SendCommand::Zone2C_LEAVE_SECTOR_USER_INFO, sizeof(UserNumPacket));
	userNumPacket_Exit->userIndex = _user->GetInfo()->userInfo.userID;
	m_sendBuffer->Write(userNumPacket_Exit->size);

	//자신이 있던 섹터 범위 내의 다른 유저들에게 자신이 나갔다고 알려주는 함수
	SectorSendAll(&m_leaveSectorsVec, 
		reinterpret_cast<char*>(userNumPacket_Exit), userNumPacket_Exit->size);

	//내가 있던 섹터 범위 내의 다른 유저들의 리스트를 보내주는 함수(클라에서 안보이게 하기 위해)
	SendInvisibleUserList(_user);
	
	//내가 있던 섹터 범위 내의 몬스터들의 리스트를 보내주는 함수(클라에서 안보이게 하기 위해)
	SendInvisibleMonsterList(_user);
}

void Field::SendInvisibleUserList(User* _user)
{
	int tempNum = 0;

	UserListPacket* userListPacket_Invisible =
		reinterpret_cast<UserListPacket*>(_user->GetSendBuffer()->
			GetBuffer(sizeof(UserListPacket)));
	userListPacket_Invisible->userNum = 0;

	for (const auto& tempSector : m_leaveSectorsVec)
	{
		if (tempSector == nullptr) break;

		if (tempSector->Manager<User>::GetItemList()->size() <= 0) continue;

		std::list<User*> tempList = *tempSector->Manager<User>::GetItemList();

		for (const auto& element : tempList)
		{
			if (element == nullptr) break;

			if (element->GetIsTestClient()) continue;

			userListPacket_Invisible->info[tempNum].userInfo = element->GetInfo()->userInfo;
			userListPacket_Invisible->info[tempNum].unitInfo = element->GetInfo()->unitInfo;

			tempNum++;
			userListPacket_Invisible->userNum++;
		}
	}

	userListPacket_Invisible->size = (sizeof(BasicInfo) * userListPacket_Invisible->userNum)
		+ sizeof(WORD) + sizeof(Packet);
	userListPacket_Invisible->Init(SendCommand::Zone2C_USER_LIST_INVISIBLE, userListPacket_Invisible->size);

	_user->GetSendBuffer()->Write(userListPacket_Invisible->size);
	_user->Send(reinterpret_cast<char*>(userListPacket_Invisible), userListPacket_Invisible->size);
}

void Field::SendInvisibleMonsterList(User* _user)
{
	int tempNum = 0;

	MonsterInfoListPacket* monsterInfoListPacket_Invisible =
		reinterpret_cast<MonsterInfoListPacket*>(_user->GetSendBuffer()->
			GetBuffer(sizeof(MonsterInfoListPacket)));

	monsterInfoListPacket_Invisible->monsterNum = 0;

	for (const auto& tempSector : m_leaveSectorsVec)
	{
		if (tempSector == nullptr) break;

		if (tempSector->Manager<Monster>::GetItemList()->size() <= 0) continue;

		std::list<Monster*> tempList = *tempSector->Manager<Monster>::GetItemList();

		for (const auto& element : tempList)
		{
			monsterInfoListPacket_Invisible->info[tempNum] = element->GetInfo();

			tempNum++;
			monsterInfoListPacket_Invisible->monsterNum++;
		}
	}

	monsterInfoListPacket_Invisible->size = (sizeof(MonsterInfo) * monsterInfoListPacket_Invisible->monsterNum)
		+ sizeof(WORD) + sizeof(Packet);
	monsterInfoListPacket_Invisible->Init(SendCommand::Zone2C_MONSTER_INFO_LIST_INVISIBLE, monsterInfoListPacket_Invisible->size);

	_user->GetSendBuffer()->Write(monsterInfoListPacket_Invisible->size);
	_user->Send(reinterpret_cast<char*>(monsterInfoListPacket_Invisible), monsterInfoListPacket_Invisible->size);
}

void Field::EnterSector(User* _user)
{
	UserPositionPacket* userPositionPacket_Enter = 
		reinterpret_cast<UserPositionPacket*>(m_sendBuffer->
			GetBuffer(sizeof(UserPositionPacket)));
	userPositionPacket_Enter->Init(SendCommand::Zone2C_ENTER_SECTOR_USER_INFO, sizeof(UserPositionPacket));
	userPositionPacket_Enter->userIndex = _user->GetInfo()->userInfo.userID;
	userPositionPacket_Enter->position = _user->GetInfo()->unitInfo.position;
	m_sendBuffer->Write(userPositionPacket_Enter->size);

	/*UserNumPacket* userNumPacket_Enter =
		reinterpret_cast<UserNumPacket*>(m_sendBuffer->
			GetBuffer(sizeof(UserNumPacket)));
	userNumPacket_Enter->Init(SendCommand::Zone2C_ENTER_SECTOR_USER_INFO, sizeof(UserNumPacket));
	userNumPacket_Enter->userIndex = _user->GetInfo()->userInfo.userID;
	m_sendBuffer->Write(userNumPacket_Enter->size);*/

	//내가 들어온 섹터 범위 내의 다른 유저들에게 자신이 들어왔다고 알려주는 함수
	SectorSendAll(&m_enterSectorsVec, 
		reinterpret_cast<char*>(userPositionPacket_Enter), userPositionPacket_Enter->size);

	//내가 들어온 섹터 범위 내의 다른 유저들의 리스트를 보내주는 함수(클라에서 보이게 하기 위해)
	SendVisibleUserList(_user);
}

void Field::SendVisibleUserList(User* _user)
{
	int tempNum = 0;

	UserListPacket* userListPacket_Visible =
		reinterpret_cast<UserListPacket*>(_user->GetSendBuffer()->
			GetBuffer(sizeof(UserListPacket)));
	userListPacket_Visible->userNum = 0;

	for (const auto& tempSector : m_enterSectorsVec)
	{
		if (tempSector == nullptr) break;

		if (tempSector->Manager<User>::GetItemList()->size() <= 0) continue;

		std::list<User*> tempList = *tempSector->Manager<User>::GetItemList();

		for (const auto& element : tempList)
		{
			if (element == nullptr) break;

			if (element->GetIsTestClient()) continue;

			userListPacket_Visible->info[tempNum].userInfo = element->GetInfo()->userInfo;
			userListPacket_Visible->info[tempNum].unitInfo = element->GetInfo()->unitInfo;

			tempNum++;
			userListPacket_Visible->userNum++;
		}
	}

	userListPacket_Visible->size = (sizeof(BasicInfo) * userListPacket_Visible->userNum)
		+ sizeof(WORD) + sizeof(Packet);
	userListPacket_Visible->Init(SendCommand::Zone2C_USER_LIST_VISIBLE, userListPacket_Visible->size);

	_user->GetSendBuffer()->Write(userListPacket_Visible->size);
	_user->Send(reinterpret_cast<char*>(userListPacket_Visible), userListPacket_Visible->size);
}

void Field::SendVisibleMonsterList(User* _user)
{
	int tempNum = 0;

	MonsterInfoListPacket* monsterInfoListPacket_Visible =
		reinterpret_cast<MonsterInfoListPacket*>(_user->GetSendBuffer()->
			GetBuffer(sizeof(MonsterInfoListPacket)));

	monsterInfoListPacket_Visible->monsterNum = 0;

	for (const auto& tempSector : m_enterSectorsVec)
	{
		if (tempSector == nullptr) break;

		if (tempSector->Manager<Monster>::GetItemList()->size() <= 0) continue;

		std::list<Monster*> tempList = *tempSector->Manager<Monster>::GetItemList();

		for (const auto& element : tempList)
		{
			monsterInfoListPacket_Visible->info[tempNum] = element->GetInfo();

			tempNum++;
			monsterInfoListPacket_Visible->monsterNum++;
		}
	}

	monsterInfoListPacket_Visible->size = (sizeof(MonsterInfo) * monsterInfoListPacket_Visible->monsterNum)
		+ sizeof(WORD) + sizeof(Packet);
	monsterInfoListPacket_Visible->Init(SendCommand::Zone2C_MONSTER_INFO_LIST_VISIBLE, monsterInfoListPacket_Visible->size);

	_user->GetSendBuffer()->Write(monsterInfoListPacket_Visible->size);
	_user->Send(reinterpret_cast<char*>(monsterInfoListPacket_Visible), monsterInfoListPacket_Visible->size);
}