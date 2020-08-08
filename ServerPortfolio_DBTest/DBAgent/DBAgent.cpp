#include "DBAgent.h"

#include "MainThread.h"

DBAgent::DBAgent()
{
	
}

DBAgent::~DBAgent()
{
	DisConnect();
}

void DBAgent::OnConnect()
{
	ClientSession::OnConnect();
}

void DBAgent::DisConnect()
{
	ClientSession::DisConnect();

	int errorNum = WSAGetLastError();
	if (errorNum != 0)
	{
		printf("%d Error \n", errorNum);
	}

	printf("===== [ close socket : %d ] ===== \n", m_socket);

	shutdown(m_socket, SD_BOTH);
	//shutdown ���� close
	closesocket(m_socket); //������ �ٸ� ������ �ص��ȴ�.

	//MainThread::getSingleton()->AddToDisConnectQueue(this);
}

void DBAgent::Reset()
{
	ClientSession::Reset();
}

void DBAgent::OnRecv()
{
	int tempNum = 20;

	while (1)
	{
		Packet* packet = reinterpret_cast<Packet*>(m_receiver->GetPacket());

		if (packet == nullptr) break;

		MainThread::getSingleton()->AddToRecvQueue(packet);

		tempNum--;

		if (tempNum <= 0)
		{
			tempNum = 20;

			break;
		}
	}
}