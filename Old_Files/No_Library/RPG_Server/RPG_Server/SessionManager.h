#pragma once
#include <algorithm>
#include <set>

#include "Session.h"
#include "ObjectPool.h"
#include "ManagerFrame_List.h"
#include "CriticalSectionClass.h"

//=====================================================

//SessionManager

//Session���� �����ϱ����� �Ŵ���

//=====================================================

class SessionManager : public Manager_List<Session>
{
private:
	CRITICAL_SECTION		m_cs;

	ObjectPool<Session>		m_objectPool;

	set<int>				m_idSet;

public:
	SessionManager() {}
	~SessionManager() {
		DeleteCriticalSection(&m_cs);

		m_idSet.clear();
	}

	void Init(ObjectPool<Session>* _pool);
	Session* CreateSession();
	void DeleteSession(Session* _t);
	void AddSessionList(Session* _t);

	void AddSessionID(int _num);
	void DeleteSessionID(int _num);
	bool FindSessionID(int _num);
};