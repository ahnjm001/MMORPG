#include "stdafx.h"
#include "RPGServer.h"

int main()
{
	_wsetlocale(LC_ALL, L"korean");      //����ȭ ������ ���������� ����
	wcout.imbue(locale("korean"));       //��½� �κ��� ����
	wcin.imbue(locale("korean"));        //�Է½� �κ��� ����

	//======================================================

	RPGServer* rpgServer = new RPGServer();
	rpgServer->Init();

	WSACleanup();

	return 0;
}