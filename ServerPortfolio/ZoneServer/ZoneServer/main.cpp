//���̺귯�� �߰�
#ifdef _DEBUG
#pragma comment (lib, "../x64/Debug/_ServerLibrary.lib")
#else
#pragma comment (lib, "../x64/Release/_ServerLibrary.lib")
#endif

#include <memory>

#include "../ServerLibrary/HeaderFiles/FileLog.h"
#include "../ServerLibrary/HeaderFiles/Utils.h"
#include "../ServerLibrary/HeaderFiles/IOCPClass.h"

#include "Acceptor.h"

#include "ZoneServer.h"

WSADATA m_wsaData;

IOCPClass* iocpClass;
Acceptor* acceptor;

int portNum;

int main()
{
	std::cout << "<< �� ��° Zone���� �Է�(1����) >>" << endl;
	std::cin >> portNum;
	portNum += 30005;
	//port�� 30006����

	system("cls");

	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
	{
		printf("[ Failed WSAStartup() ] \n");

		return 0;
	}

	TRYCATCH(iocpClass = new IOCPClass());
	if (iocpClass->IsFailed()) return false;

	ZoneServer* zoneServer = new ZoneServer(*iocpClass);
	if (!zoneServer->Start())
	{
		MYDEBUG("[ Zone Server Initializing Fail ]\n");

		WSACleanup();

		return 0;
	}

	TRYCATCH(acceptor = new Acceptor("192.168.0.13", portNum,
		iocpClass->GetIOCPHandle(), 0));
	if (acceptor->IsFailed()) return false;
	iocpClass->Associate(acceptor->GetListenSocket(),
		(unsigned long long)acceptor->GetListenSocket());

	MYDEBUG("[ �� ���� Port : %d ]\n", portNum);

	WaitForSingleObject(MainThread::getSingleton()->GetHandle(), INFINITE);

	delete acceptor;
	delete zoneServer;
	delete iocpClass;

	WSACleanup();

	return 0;
}