#include "stdafx.h"
#include "ZoneServer.h"

int main()
{
	_wsetlocale(LC_ALL, L"korean");      //����ȭ ������ ���������� ����
	wcout.imbue(locale("korean"));       //��½� �κ��� ����
	wcin.imbue(locale("korean"));        //�Է½� �κ��� ����

	//======================================================

	ZoneServer* zoneServer = new ZoneServer();
	zoneServer->Init();

	WSACleanup();

	return 0;
}