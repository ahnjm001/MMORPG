#include "stdafx.h"
#include "App.h"

int num;
int portNum_login;
int portNum_zone;

int main()
{
	std::cout << "<< �� ��° Zone�� �������� �Է�(0����) >>" << endl;
	std::cin >> num;

	system("cls");

	App app;

	IpEndPoint ipEndPoint;
	ipEndPoint = IpEndPoint("211.221.147.29", 30006 + num);

	app.Init(num);

	app.Begin(ipEndPoint);

	WaitForSingleObject(app.GetHandle(), INFINITE);

	return 1;
}