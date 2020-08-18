//���̺귯�� �߰�
#pragma comment (lib, "../x64/Debug/1.ServerLibrary.lib")

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

#include <memory>

#include "../ServerLibrary/HeaderFiles/FileLog.h"
#include "../ServerLibrary/HeaderFiles/Utils.h"
#include "../ServerLibrary/HeaderFiles/IOCPClass.h"

#include "Acceptor.h"
#include "LogInServer.h"

WSADATA m_wsaData;

IOCPClass* iocpClass;
Acceptor* acceptor;

int main()
{
	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
	{
		printf("[ Failed WSAStartup() ] \n");

		return 0;
	}

	TRYCATCH(iocpClass = new IOCPClass());
	if (iocpClass->IsFailed()) return false;

	LogInServer* logInServer = new LogInServer(*iocpClass);
	if (!logInServer->Start())
	{
		printf("[ LogInServer Initializing Fail ]\n");

		WSACleanup();

		return 0;
	}

	TRYCATCH(acceptor = new Acceptor("192.168.0.13", 30003,
		iocpClass->GetIOCPHandle(), 0));
	if (acceptor->IsFailed()) return false;
	iocpClass->Associate(acceptor->GetListenSocket(),
		(unsigned long long)acceptor->GetListenSocket());

	WaitForSingleObject(MainThread::getSingleton()->GetHandle(), INFINITE);

	delete acceptor;
	delete iocpClass;

	WSACleanup();

	return 0;
}