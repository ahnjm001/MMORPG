#include "LogInConnector.h"

LogInConnector::LogInConnector()
{
}

LogInConnector::~LogInConnector()
{
	DisConnect();
}

bool LogInConnector::Connect(const char* _ip, const unsigned short _portNum, IOCPClass* _iocpClass)
{
	TRYCATCH(m_ipEndPoint = IpEndPoint(_ip, _portNum));

	// 1. ���ϻ���
	m_socket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (m_socket == INVALID_SOCKET)
	{
		MYDEBUG("[ Error - Invalid socket ]\n");
		FILELOG("[ Error - Invalid socket ]");

		return false;
	}

	// 2. �����û
	if (connect(m_socket, (SOCKADDR*)&m_ipEndPoint, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		MYDEBUG("[ Error - Fail to connect ]\n");
		FILELOG("[ Error - Fail to connect ]");
		// 4. ��������
		closesocket(m_socket);
		// Winsock End
		WSACleanup();

		return false;
	}
	else
	{
		MYDEBUG("[ LogInServer Connecting ]\n");

		m_IOCPClass = _iocpClass;
		m_IOCPClass->Associate(m_socket, (unsigned long long)this);

		return true;
	}
}

void LogInConnector::OnConnect()
{
	ClientSession::OnConnect();

	m_start = std::chrono::high_resolution_clock::now();

	Sleep(100);

	Packet* helloPacket = new Packet();
	helloPacket->Init(SendCommand::Zone2LogIn_HELLO, sizeof(Packet));

	SharedPointer<char> tempPtr =
		SharedPointer<char>(reinterpret_cast<char*>(helloPacket));
	AddToSendQueue(tempPtr);
}

void LogInConnector::DisConnect()
{
	ClientSession::DisConnect();

	int errorNum = WSAGetLastError();
	if (errorNum != 0)
	{
		MYDEBUG("===== [ close socket : %ld, %d Error - LogInConnector ] ===== \n", m_socket, errorNum);
	}
	else
	{
		MYDEBUG("===== [ close socket : %ld - LogInConnector ] ===== \n", m_socket);
	}

	shutdown(m_socket, SD_BOTH);
	//shutdown ���� close
	closesocket(m_socket);

	Connect("211.221.147.29", 30004, m_IOCPClass);
	OnConnect();
}

void LogInConnector::Reset()
{
	ClientSession::Reset();
}

void LogInConnector::OnRecv()
{
	int tempNum = 20;

	while (1)
	{
		Packet* packet = reinterpret_cast<Packet*>(m_receiver->GetPacket());

		if (packet == nullptr) break;

		MainThread::getSingleton()->AddToLogInServerPacketQueue(packet);

		tempNum--;

		if (tempNum <= 0)
		{
			tempNum = 20;

			break;
		}
	}
}

void LogInConnector::SendDisConnectUser(int _num)
{
	UserNumPacket* DisConnectUserPacket = new UserNumPacket();
	DisConnectUserPacket->Init(SendCommand::Zone2LogIn_DISCONNECT_USER, sizeof(UserNumPacket));
	DisConnectUserPacket->userIndex = _num;
	//m_sendBuffer->Write(DisConnectUserPacket->size);

	SharedPointer<char> tempPtr =
		SharedPointer<char>(reinterpret_cast<char*>(DisConnectUserPacket));
	AddToSendQueue(tempPtr);
}

void LogInConnector::HeartBeat()
{
	auto m_end = std::chrono::high_resolution_clock::now();
	auto m_durationSec = std::chrono::duration_cast<std::chrono::seconds>(m_end - m_start);

	if (m_durationSec.count() > 30)
	{
		MYDEBUG("[ Login Server�� ���� ���� ] \n");

		DisConnect();

		Connect("211.221.147.29", 30004, m_IOCPClass);
	}

	Packet* heartBeatPacket = new Packet();
	heartBeatPacket->Init(SendCommand::Zone2Login_HEARTBEAT, sizeof(Packet));

	MYDEBUG("[ LoginServer HeartBeat ] \n");
	SharedPointer<char> tempPtr =
		SharedPointer<char>(reinterpret_cast<char*>(heartBeatPacket));
	AddToSendQueue(tempPtr);
}