#include "SessionManager.h"

void SessionManager::Init()
{
	InitializeCriticalSection(&m_cs);

	m_checkingSessionsNum = 0;
}

void SessionManager::AddObject(Session* _t)
{
	m_objectPool.AddObject(_t);
}

void SessionManager::CheckingAccept()
{
	Session* tempSession;

	while (m_checkingSessionsNum < 5)
	{
		tempSession = m_objectPool.PopObject();

		//������Ʈ Ǯ�� ���̻� �����ִ� Session�� ����.
		if (tempSession == nullptr)
		{
			printf("[ ���� ��� ���� ���� �߰� �Ұ��� - ���� Session�� ���� ]");

			break;
		}

		tempSession->StartAccept();
		InterlockedIncrement(&m_checkingSessionsNum);
	}

	if (m_checkingSessionsNum > 0)
	{
		printf("[ ���� ��� ������ �� : %d ] \n", m_checkingSessionsNum);
	}
	else
	{
		printf("[ ��ȭ - ���� ��� ������ �� : 0 ]\n");
	}
}

Session* SessionManager::PopSession()
{
	return m_objectPool.PopObject(); //�ȿ��� lock�ϴ���
}

void SessionManager::AddSessionList(Session* _t)
{
	CSLock csLock(m_cs);

	AddItem(_t);
	m_checkingSessionsNum -= 1;

	CheckingAccept();
}

void SessionManager::ReturnSessionList(Session* _t)
{
	if (DeleteItem(_t))
	{
		m_objectPool.ReturnObject(_t);
	}
}

void SessionManager::AddSessionID(int _num)
{
	m_idSet.insert(_num);
}

void SessionManager::DeleteSessionID(int _num)
{
	CSLock csLock(m_cs);

	if (FindSessionID(_num))
	{
		m_idSet.erase(_num);
	}
}

bool SessionManager::FindSessionID(int _num)
{
	set<int>::iterator FindIter = m_idSet.find(_num);

	if (FindIter != m_idSet.end())
	{
		return true;
	}

	return false;
}