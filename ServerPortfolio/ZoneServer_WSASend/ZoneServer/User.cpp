#include "User.h"
#include "Field.h"
#include "Sector.h"

#include "DBConnector.h"
#include "LogInConnector.h"

#include "MainThread.h"

User::User()
{
	m_isInHashMap = false;
	m_isAlreadyDisConnected = false;

	m_basicInfo.unitInfo.Reset();
	m_basicInfo.userInfo.Reset();

	m_tile = nullptr;
	m_sector = nullptr;
	m_field = nullptr;
	m_fieldTilesData = nullptr;

	m_isGetUserList = false;
}

User::~User()
{
	DisConnect();
}

void User::OnConnect()
{
	m_isAlreadyDisConnected = false;

	ClientSession::OnConnect();

	BOOL bVal = TRUE;
	::setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, (char *)&bVal, sizeof(BOOL));

	IsConnectedPacket* isConnectedPacket = new IsConnectedPacket();

	isConnectedPacket->Init(SendCommand::Zone2C_ISCONNECTED, sizeof(IsConnectedPacket));
	isConnectedPacket->isConnected = m_isConnected;

	SharedPointer<char> tempPtr =
		SharedPointer<char>(reinterpret_cast<char*>(isConnectedPacket));
	AddToSendQueue(tempPtr);

	//printf("[ isConnectedPacket ���� �Ϸ� ]\n");
}

void User::DisConnect()
{
	if (m_isAlreadyDisConnected || !m_isConnected) return;

	m_isAlreadyDisConnected = true;

	ClientSession::DisConnect();

	int errorNum = WSAGetLastError();
	if (errorNum != 0)
	{
		//MYDEBUG("===== [ close socket : %ld, %d Error ] ===== \n", m_socket, errorNum);
	}
	else
	{
		//MYDEBUG("===== [ close socket : %ld ] ===== \n", m_socket);
	}

	shutdown(m_socket, SD_BOTH);
	//shutdown ���� close
	closesocket(m_socket); //������ �ٸ� ������ �ص��ȴ�.

	//LogInConnector::getSingleton()->SendDisConnectUser(m_basicInfo.userInfo.userID);

	MainThread::getSingleton()->AddToDisConnectQueue(this);
}

void User::DisConnect_ChangeZone(int _num)
{
	if (m_isAlreadyDisConnected || !m_isConnected) return;

	ClientSession::DisConnect();

	int errorNum = WSAGetLastError();
	if (errorNum != 0)
	{
		MYDEBUG("===== [ close socket : %ld, %d Error ] ===== \n", m_socket, errorNum);
	}
	else
	{
		MYDEBUG("===== [ close socket : %ld ] ===== \n", m_socket);
	}

	shutdown(m_socket, SD_BOTH);
	//shutdown ���� close
	closesocket(m_socket); //������ �ٸ� ������ �ص��ȴ�.

	MainThread::getSingleton()->AddToDisConnectQueue(this);

	UserNumPacket* ReadyToChangeZone = new UserNumPacket();
	ReadyToChangeZone->Init(SendCommand::Zone2Login_READYTOCHANGEZONE, sizeof(UserNumPacket));
	ReadyToChangeZone->userIndex = m_basicInfo.userInfo.userID;
	//m_sendBuffer->Write(ReadyToChangeZone->size);

	SharedPointer<char> tempPtr =
		SharedPointer<char>(reinterpret_cast<char*>(ReadyToChangeZone));
	LogInConnector::getSingleton()->AddToSendQueue(tempPtr);
}

void User::Reset()
{
	ClientSession::Reset();

	m_isInHashMap = false;

	m_basicInfo.unitInfo.Reset();
	m_basicInfo.userInfo.Reset();

	m_field = nullptr;

	m_isGetUserList = false;

	m_sector = nullptr;

	m_basicInfo.unitInfo.fieldNum = 0;
}

void User::OnRecv()
{
	int tempNum = 20;

	while (1)
	{
		Packet* packet = reinterpret_cast<Packet*>(m_receiver->GetPacket());

		if (packet == nullptr) break;

		MainThread::getSingleton()->AddToUserPacketQueue({ this, packet });

		tempNum--;

		if (tempNum <= 0)
		{
			tempNum = 20;

			break;
		}
	}
}

void User::UpdateInfo()
{
	MYDEBUG("[ %d user - INFO UPDATE TO DATABASE START ] \n", m_basicInfo.userInfo.userID);

	UpdateUserPacket* updateUserPacket = new UpdateUserPacket();
	updateUserPacket->Init(SendCommand::Zone2DB_UPDATE_USER, sizeof(UpdateUserPacket));
	updateUserPacket->userIndex = m_basicInfo.userInfo.userID;
	updateUserPacket->unitInfo = m_basicInfo.unitInfo;
	updateUserPacket->socket = m_socket;

	SharedPointer<char> tempPtr =
		SharedPointer<char>(reinterpret_cast<char*>(updateUserPacket));
	DBConnector::getSingleton()->AddToSendQueue(tempPtr);
}

void User::Death()
{
	m_basicInfo.unitInfo.state = STATE::DEATH;

	//�׾��ٰ� ��Ŷ �����������.
	UserNumPacket* userNumPacket = new UserNumPacket();

	userNumPacket->userIndex = m_basicInfo.userInfo.userID;
	userNumPacket->Init(SendCommand::Zone2C_USER_DEATH, sizeof(UserNumPacket));
	//m_sendBuffer->Write(userNumPacket->size);

	SharedPointer<char> tempPtr =
		SharedPointer<char>(reinterpret_cast<char*>(userNumPacket));
	m_field->SectorSendAll(m_sector->GetRoundSectorsVec(), tempPtr);
}

void User::Respawn(const VECTOR2& _spawnPosition)
{
	m_basicInfo.unitInfo.state = STATE::IDLE;

	m_basicInfo.unitInfo.hp.currentValue = m_basicInfo.unitInfo.hp.maxValue;
	m_basicInfo.unitInfo.mp.currentValue = m_basicInfo.unitInfo.mp.maxValue;

	m_basicInfo.unitInfo.SetUnitPosition(_spawnPosition.x, _spawnPosition.y);
	m_tile = m_fieldTilesData->GetTile(
		static_cast<int>(m_basicInfo.unitInfo.position.x),
		static_cast<int>(m_basicInfo.unitInfo.position.y));

	SessionInfoPacket* sessionInfoPacket = new SessionInfoPacket();
	sessionInfoPacket->Init(SendCommand::Zone2C_USER_REVIVE, sizeof(SessionInfoPacket));
	//m_sendBuffer->Write(sessionInfoPacket->size);

	sessionInfoPacket->info.userInfo = m_basicInfo.userInfo;
	sessionInfoPacket->info.unitInfo = m_basicInfo.unitInfo;

	SharedPointer<char> tempPtr =
		SharedPointer<char>(reinterpret_cast<char*>(sessionInfoPacket));
	m_field->SectorSendAll(m_sector->GetRoundSectorsVec(), tempPtr);
}

void User::Hit(int _index, int _damage)
{
	if (m_basicInfo.unitInfo.state == STATE::DEATH) return;

	int damage = _damage - m_basicInfo.unitInfo.def;
	if (damage < 0) damage = 0;

	m_basicInfo.unitInfo.hp.currentValue -= damage;

	UserHitPacket* userHitPacket = new UserHitPacket();

	userHitPacket->userIndex = m_basicInfo.userInfo.userID;
	userHitPacket->monsterIndex = _index;
	userHitPacket->damage = damage;
	userHitPacket->Init(SendCommand::Zone2C_USER_HIT, sizeof(UserHitPacket));
	//m_sendBuffer->Write(userHitPacket->size);

	SharedPointer<char> tempPtr =
		SharedPointer<char>(reinterpret_cast<char*>(userHitPacket));
	m_field->SectorSendAll(m_sector->GetRoundSectorsVec(), tempPtr);

	if (m_basicInfo.unitInfo.hp.currentValue <= 0)
	{
		Death();
	}
}

void User::PlusExp(int _exp)
{
	m_basicInfo.unitInfo.exp.currentValue += _exp;

	UserExpPacket* userExpPacket = new UserExpPacket();

	userExpPacket->exp = _exp;
	userExpPacket->Init(SendCommand::Zone2C_USER_PLUS_EXP, sizeof(UserExpPacket));
	//m_sendBuffer->Write(userExpPacket->size);

	SharedPointer<char> tempPtr =
		SharedPointer<char>(reinterpret_cast<char*>(userExpPacket));
	AddToSendQueue(tempPtr);

	//���� ������ üũ�������
	if (m_basicInfo.unitInfo.exp.currentValue >= m_basicInfo.unitInfo.exp.maxValue)
	{
		LevelUp();
	}
}

void User::LevelUp()
{
	int tempExp = m_basicInfo.unitInfo.exp.currentValue - m_basicInfo.unitInfo.exp.maxValue;

	m_basicInfo.unitInfo.level++;
	m_basicInfo.unitInfo.exp.currentValue = tempExp;
	m_basicInfo.unitInfo.hp.maxValue += 50;
	m_basicInfo.unitInfo.hp.currentValue = m_basicInfo.unitInfo.hp.maxValue;
	m_basicInfo.unitInfo.mp.currentValue = m_basicInfo.unitInfo.mp.maxValue;
	m_basicInfo.unitInfo.atk += 5;
	m_basicInfo.unitInfo.def += 2;

	UserNumPacket* userNumPacket = new UserNumPacket();

	userNumPacket->userIndex = m_basicInfo.userInfo.userID;
	userNumPacket->Init(SendCommand::Zone2C_USER_LEVEL_UP, sizeof(UserNumPacket));
	//m_sendBuffer->Write(userNumPacket->size);

	SharedPointer<char> tempPtr =
		SharedPointer<char>(reinterpret_cast<char*>(userNumPacket));
	m_field->SectorSendAll(m_sector->GetRoundSectorsVec(), tempPtr);

	//�ٽ� ������ üũ
	if (m_basicInfo.unitInfo.exp.currentValue >= m_basicInfo.unitInfo.exp.maxValue)
	{
		LevelUp();
	}
}

void User::RequestUserInfo(int _num)
{
	m_basicInfo.userInfo.userID = _num;

	RequireUserInfoPacket_DBAgent* RequireUserInfoPacket =
		new RequireUserInfoPacket_DBAgent();
	RequireUserInfoPacket->Init(SendCommand::Zone2DB_REQUEST_USER_DATA, sizeof(RequireUserInfoPacket_DBAgent));
	//m_sendBuffer->Write(RequireUserInfoPacket->size);
	RequireUserInfoPacket->userIndex = m_basicInfo.userInfo.userID;
	RequireUserInfoPacket->socket = m_socket;

	SharedPointer<char> tempPtr =
		SharedPointer<char>(reinterpret_cast<char*>(RequireUserInfoPacket));
	DBConnector::getSingleton()->AddToSendQueue(tempPtr);
}

//�⺻���� ������ ������ �����ش�. DB�� ������ ���⼭ �ҷ��ͼ� ������.
void User::SendInfo(GetSessionInfoPacket* _packet)
{
	int tempNum = m_basicInfo.userInfo.userID;
	m_basicInfo.userInfo = _packet->info.userInfo;
	m_basicInfo.userInfo.userID = tempNum;
	m_basicInfo.unitInfo = _packet->info.unitInfo;

	SessionInfoPacket* sessionInfoPacket = new SessionInfoPacket();
	sessionInfoPacket->Init(SendCommand::Zone2C_REGISTER_USER, sizeof(SessionInfoPacket));
	//m_sendBuffer->Write(sessionInfoPacket->size);

	sessionInfoPacket->info.userInfo = m_basicInfo.userInfo;
	sessionInfoPacket->info.unitInfo = m_basicInfo.unitInfo;

	SharedPointer<char> tempPtr =
		SharedPointer<char>(reinterpret_cast<char*>(sessionInfoPacket));
	AddToSendQueue(tempPtr);

	//MYDEBUG("[ REGISTER_USER ���� �Ϸ� ]\n");
}

void User::RequestUserInfoFailed()
{
	MYDEBUG("[ GetUserInfo Failed, Disconnect %ld Socket User ]\n", m_socket);

	DisConnect();
}

//�޾ƿ� Field�� Num���� User�� �־��ְ� �Ϸ��ߴٴ� ��Ŷ ����. ���� Ŭ�󿡼� �� ��ȯ���ٵ�
void User::EnterField(Field *_field, int _fieldNum, VECTOR2* _spawnPosition)
{
	EnterFieldPacket* enterFieldPacket = new EnterFieldPacket();
	enterFieldPacket->fieldNum = _fieldNum;

	enterFieldPacket->position = *_spawnPosition;
	m_basicInfo.unitInfo.position.x = _spawnPosition->x;
	m_basicInfo.unitInfo.position.y = _spawnPosition->y;

	enterFieldPacket->Init(SendCommand::Zone2C_ENTER_FIELD, sizeof(EnterFieldPacket));
	//m_sendBuffer->Write(enterFieldPacket->size);

	m_field = _field;
	m_basicInfo.unitInfo.fieldNum = m_field->GetFieldNum();
	m_fieldTilesData = m_field->GetTilesData();

	m_tile = m_fieldTilesData->GetTile(
		static_cast<int>(m_basicInfo.unitInfo.position.x),
		static_cast<int>(m_basicInfo.unitInfo.position.y));

	SharedPointer<char> tempPtr =
		SharedPointer<char>(reinterpret_cast<char*>(enterFieldPacket));
	AddToSendQueue(tempPtr);

	MYDEBUG("[ ENTER_FIELD ]\n");
}

void User::SetPosition(Position& _position)
{
	float distance = VECTOR2(m_basicInfo.unitInfo.position - _position).Magnitude();

	/*printf("[%d User : SetPosition (%f, %f) - dir : %f ]\n",
		m_basicInfo.userInfo.userID, _position.x, _position.y, distance);*/

	m_basicInfo.unitInfo.position = _position;
	m_tile = m_fieldTilesData->GetTile(
		static_cast<int>(m_basicInfo.unitInfo.position.x),
		static_cast<int>(m_basicInfo.unitInfo.position.y));
}

bool User::CompareSector(Sector* _sector)
{
	if (m_sector == nullptr) return false;

	if (m_sector == _sector)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//�׽�Ʈ��
void User::TestClientEnterField(Field* _field, int _fieldNum, int _dummyNum, VECTOR2* _spawnPosition)
{
	m_isTestClient = true;

	m_field = _field;
	m_basicInfo.unitInfo.fieldNum = m_field->GetFieldNum();
	m_fieldTilesData = m_field->GetTilesData();

	m_basicInfo.userInfo.userID = _dummyNum;
	strcpy_s(m_basicInfo.userInfo.userName, "TestPlayer");

	m_basicInfo.unitInfo.SetUnitInfo(IDLE, 1,
		100, 100, 100, 100, 100, 0, 20, 0);
	m_basicInfo.unitInfo.SetUnitPosition(0, 0);

	m_basicInfo.unitInfo.position.x = _spawnPosition->x;
	m_basicInfo.unitInfo.position.y = _spawnPosition->y;

	m_tile = m_fieldTilesData->GetTile(
		static_cast<int>(m_basicInfo.unitInfo.position.x),
		static_cast<int>(m_basicInfo.unitInfo.position.y));

	SessionInfoPacket* sessionInfoPacket = new SessionInfoPacket();
	sessionInfoPacket->Init(SendCommand::Zone2C_REGISTER_TEST_USER, sizeof(SessionInfoPacket));

	sessionInfoPacket->info.userInfo = m_basicInfo.userInfo;
	sessionInfoPacket->info.unitInfo = m_basicInfo.unitInfo;

	SharedPointer<char> tempPtr =
		SharedPointer<char>(reinterpret_cast<char*>(sessionInfoPacket));
	AddToSendQueue(tempPtr);
}