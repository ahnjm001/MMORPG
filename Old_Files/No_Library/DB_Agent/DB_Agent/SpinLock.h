#pragma once
#include <Windows.h>

//200601
//�� ���°�?
//�Ϲ� lock�� spin lock�� ����
//Interlocked �迭 �Լ��� ���� �� �� ����

class SpinLock
{
private:
	int m_use;
public:
	SpinLock() {
		m_use = 0;
	}
	~SpinLock() {}

	void Enter()
	{
		//��� ����
		while (true)
		{
			if (InterlockedExchange((LPLONG)&m_use, 1) == 0) break;
			Sleep(0);
		}
	}

	void Leave()
	{
		InterlockedExchange((LPLONG)&m_use, 0);
	}
};