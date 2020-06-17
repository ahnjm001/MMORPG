#pragma once
#include <set>

#include "Session.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/Thread.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/ObjectPool.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/ManagerFrame_List.h"
#include "../ServerLibrary/HeaderFiles/CriticalSection.h"

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

	LONG					m_checkingSessionsNum;

public:
	SessionManager() {}
	~SessionManager() {
		DeleteCriticalSection(&m_cs);

		m_idSet.clear();
	}

	void Init();

	void AddObject(Session* _t);

	void CheckingAccept();

	Session* PopSession();
	void AddSessionList(Session* _t);
	void ReturnSessionList(Session* _t);
	void AddSessionID(int _num);
	void DeleteSessionID(int _num);
	bool FindSessionID(int _num);

	void AddToSwapQueue(Session* _t);
	void CheckingSwapQueue();

	void LoopRun();

	ObjectPool<Session>* GetObjectPool() { return &m_objectPool; }
};