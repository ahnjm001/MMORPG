#include "ZoneServer.h"

ZoneServer::~ZoneServer()
{
	if (m_userManager != nullptr) delete m_userManager;
	if (m_fieldManager != nullptr) delete m_fieldManager;

	LOG::FileLog("../LogFile.txt", __FILENAME__, __LINE__, "�� ���� ���� ����");
}

bool ZoneServer::Start()
{
	MYDEBUG("[ �� ���� ���� ���� ]\n");
	LOG::FileLog("../LogFile.txt", __FILENAME__, __LINE__, "�� ���� ���� ����");

	if (!MainThread::getSingleton()->Init())
	{
		return false;
	}

	TRYCATCH(m_userManager = new UserManager());

	User* user;
	for (int i = 0; i < USERMAXCOUNT; i++)
	{
		TRYCATCH(user = new User());
		m_userManager->AddObject(user);
	}

	MYDEBUG("[ User Max Count : %d ]\n", m_userManager->GetObjectPool()->GetSize());

	if (!DBConnector::getSingleton()->Connect("211.221.147.29", 30002, &m_IOCPClass))
	{
		return false;
	}	
	DBConnector::getSingleton()->OnConnect();

	Sleep(500);

	if (!LogInConnector::getSingleton()->Connect("211.221.147.29", 30003, &m_IOCPClass))
	{
		return false;
	}
	LogInConnector::getSingleton()->OnConnect();

	Sleep(500);

	/*if (!PathFinderAgent::getSingleton()->Connect("211.221.147.29", 30009, &m_IOCPClass))
	{
		return false;
	}
	PathFinderAgent::getSingleton()->OnConnect();*/

	TRYCATCH(m_fieldManager = new FieldManager());
	if (m_fieldManager->IsFailed()) return false;

	MainThread::getSingleton()->
		SetManagers(m_userManager, m_fieldManager);

	return true;
}