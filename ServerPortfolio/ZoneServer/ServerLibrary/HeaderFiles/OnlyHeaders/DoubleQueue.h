#pragma once
#include <queue>
#include "../../HeaderFiles/CriticalSection.h"
#include "../SpinLock.h"

template<typename T>
class DoubleQueue
{
private:
	std::queue<T>			queue_1;
	std::queue<T>			queue_2;

	std::queue<T>*			primaryQueue;
	std::queue<T>*			secondaryQueue;

	SpinLock				m_spinLock;

public:
	DoubleQueue()
	{
		primaryQueue = &queue_1;
		secondaryQueue = &queue_2;
	}

	~DoubleQueue()
	{
		//�ڵ����� �޸� ������ ���ִ°�?
		//Ȥ�� �̷��� ���� ������ϳ�?
		//�˾Ƽ� �����ִ���
		/*while (!queue_1.empty())
		{
			queue_1.pop();
		}

		while (!queue_2.empty())
		{
			queue_2.pop();
		}*/
	}

	void Swap()
	{
		m_spinLock.Enter();

		std::queue<T>* temp;

		temp = primaryQueue;
		primaryQueue = secondaryQueue;
		secondaryQueue = temp;

		m_spinLock.Leave();
	}

	//�ִ°� ù��° ť������
	void AddObject(const T& _object)
	{
		m_spinLock.Enter();

		primaryQueue->push(_object);

		m_spinLock.Leave();
	}

	//���°� �ι�° ť������
	T& PopObject()
	{
		T& retVal = secondaryQueue->front();
		//�ش� �κ��� T�� ó���� ���Ŀ� ���ִ� ���� �����ϴ�.
		secondaryQueue->pop();

		return retVal;
	}

	bool IsEmpty(void)
	{
		return secondaryQueue->empty();
	}

	std::queue<T>& GetPrimaryQueue()
	{
		return *primaryQueue;
	}

	std::queue<T>& GetSecondaryQueue()
	{
		return *secondaryQueue;
	}

	int GetPrimaryQueueSize()
	{
		return primaryQueue->size();
	}

	int GetSecondaryQueueSize()
	{
		return secondaryQueue->size();
	}
};