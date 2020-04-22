#pragma once
#include "stdafx.h"

//=====================================================

//MangerFrame

//�Ŵ������� �⺻���� ��ɵ��� �������ִ� ���ø� Ŭ����

//=====================================================

template<typename T>
class Manager
{
protected:
	list<T*> m_itemList;

public:
	Manager() {}
	~Manager() {}

	void AddItem(T* _t);
	bool DeleteItem(T* _t);

	list<T*>* GetItemList();
};

template<class T>
void Manager<T>::AddItem(T * _t)
{
	m_itemList.push_back(_t);
}

template<class T>
bool Manager<T>::DeleteItem(T * _t)
{
	typename list<T*>::iterator deleteiter =
		find(m_itemList.begin(), m_itemList.end(), _t);

	if (deleteiter == m_itemList.end()) return false;

	m_itemList.erase(deleteiter);

	return true;
}

template<class T>
list<T*>* Manager<T>::GetItemList()
{
	return &m_itemList;
}