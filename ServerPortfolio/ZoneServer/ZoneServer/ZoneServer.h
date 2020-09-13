#pragma once
#include <memory>

#include "../ServerLibrary/HeaderFiles/FileLog.h"
#include "../ServerLibrary/HeaderFiles/Utils.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/IpEndPoint.h"
#include "../ServerLibrary/HeaderFiles/IOCPClass.h"

#include "User.h"
#include "UserManager.h"
#include "FieldManager.h"

#include "DBConnector.h"
#include "LogInConnector.h"
#include "PathFinderAgent.h"

#include "MainThread.h"

//https://chipmaker.tistory.com/entry/dd-1

//unique_ptr
//�޴���(�Լ�)�� ������ �ڿ��� �Ѱܾ� �� ���
//�Լ��Ķ���͸� ���̳� rvalue�� �����Ѵ�.
//rvalue�� ������ ���, �޴��ʿ��� move�� ���ؼ� �ڿ��� �������� �ڿ��̵��� �Ϸ�ȴ�.
//
//������ �ʿ����� ��� �ڿ��� ����ؾ� �� ���
//�Լ��Ķ���͸� ������ �����ؾ� �Ѵ�.

class ZoneServer
{
private:
	const int				USERMAXCOUNT = 2000;

	IOCPClass&				m_IOCPClass;

	UserManager*			m_userManager;
	FieldManager*			m_fieldManager;

public:
	ZoneServer(IOCPClass& _iocpClass)
		: m_IOCPClass(_iocpClass)
	{

	};

	~ZoneServer();

	bool Start(int _num);
};

