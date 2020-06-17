#pragma once
#include <stack>

#include "CriticalSectionClass.h"

using namespace std;

//=====================================================

//ObjectPool

//=====================================================

template<typename T>
class ObjectPool
{
private:
	CriticalSectionClass locker;
	stack<T*> objects;

public:
	int GetCount()
	{
		return objects.size();
	}

	ObjectPool() {}

	// ������Ʈ�� ����.
	~ObjectPool()
	{
		while (!objects.empty())
		{
			T* object = objects.top();
			objects.pop();
			delete object;
		}
	}

	void AddObject(T* _object)
	{
		locker.EnterLock();

		objects.push(_object);

		locker.LeaveLock();
	}

	// ������Ʈ�� ������.
	T* PopObject()
	{
		locker.EnterLock();

		// ������Ʈ�� ���ٸ� Ȯ���Ѵ�.
		if (objects.empty())
		{
			//Expand();
		}

		T* retVal = objects.top();
		objects.pop();

		locker.LeaveLock();

		//Critical Sections and return values in C++
		//https://stackoverflow.com/questions/11653062/critical-sections-and-return-values-in-c

		return retVal;
	}

	// ���� ũ�⸸ŭ ���ο� ������Ʈ�� �־� Ȯ���Ѵ�.
	/*void Expand()
	{
		locker.EnterLock();

		// �ø� ũ�⸸ŭ ���ο� ������Ʈ�� �־��ش�.
		for (int i = 0; i < maxSize; ++i)
		{
			T* newObject = new T();
			objects.push(newObject);
		}

		// �ִ� ũ�⸦ �ø���.
		maxSize += maxSize;

		locker.LeaveLock();
	}*/

	// ������Ʈ�� �����Ѵ�.
	void ReturnObject(T* _object)
	{
		//locker.EnterLock();

		objects.push(_object);

		//locker.LeaveLock();
	}
};