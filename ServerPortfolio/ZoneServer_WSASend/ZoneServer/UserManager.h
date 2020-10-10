#pragma once
#define _WINSOCKAPI_
#include "User.h"

#include "../ServerLibrary/HeaderFiles/OnlyHeaders/ObjectPool.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/ManagerFrame_List.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/ManagerFrame_UnorderedMap.h"

//=====================================================

//UserManager

//User���� �����ϱ����� �Ŵ���

//cpp���ϱ������� ���� �����Ѵ�. ������ ����?

//=====================================================

class UserManager
{
private:
	//���� ������ ������ ���� ���� ���� �����صδ� ����
	std::vector<User*>			m_userVec;
	ObjectPool<User>			m_objectPool;
	Manager_UnorderedMap<User>	m_userHashMap;
	Manager_List<User>			m_tempUserList;

public:
	UserManager();
	~UserManager();

	void AddObject(User* _t);
	void CopyToObjectPool();

	User* PopUser();
	void AddUser(User* _t);
	void ReturnUser(User* _t);

	//=============================================================
	void AddTempuser(User* _t);
	void DeleteTempUser(User* _t, bool _returnBool);

	ObjectPool<User>* GetObjectPool() { return &m_objectPool; }
	Manager_UnorderedMap<User>* GetUserHashMap() { return &m_userHashMap; }
	Manager_List<User>* GetUserList() { return &m_tempUserList; }
};