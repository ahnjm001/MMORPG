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
	SpinLock();
	~SpinLock();

	void Enter();
	void Leave();
};