#pragma once
#include <memory>

#include "../ServerLibrary/HeaderFiles/FileLog.h"
#include "../ServerLibrary/HeaderFiles/Utils.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/IpEndPoint.h"
#include "../ServerLibrary/HeaderFiles/IOCPClass.h"

#include "UserManager.h"
#include "HeartBeatThread.h"

#include "User.h"
#include "FieldManager.h"
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
	HeartBeatThread*		m_heartBeatThread;

public:
	ZoneServer(IOCPClass& _iocpClass)
		: m_IOCPClass(_iocpClass)
	{

	};

	~ZoneServer();

	bool Start();
};

