//���̺귯�� �߰�
#pragma comment (lib, "../x64/Debug/ServerLibrary.lib")

#include "ZoneServer.h"

int main()
{
	ZoneServer* zoneServer = new ZoneServer();
	if (!zoneServer->Start())
	{
		printf("[ Zone Server Initializing Fail ]\n");
	}

	WSACleanup();

	Sleep(0);

	return 0;
}