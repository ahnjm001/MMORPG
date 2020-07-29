#pragma once
#include <set>

#include "Session.h"
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
	//DB ���ν��� ���� ������(���� �α��� �ߺ� üũ�� ���ؼ��� �����)
	//Unordered Map ����� ���� ����.
	set<int>				m_idSet;

public:
	SessionManager();
	~SessionManager();

	void AddObject(Session* _t);

	Session* PopSession();
	void AddSession(Session* _t);
	void ReturnSession(Session* _t);
	void AddSessionID(int _num);
	void DeleteSessionID(int _num);
	bool FindSessionID(int _num);

	ObjectPool<Session>* GetObjectPool() { return &m_objectPool; }
};