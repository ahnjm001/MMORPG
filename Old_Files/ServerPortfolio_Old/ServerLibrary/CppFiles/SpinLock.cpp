#include "../HeaderFiles/SpinLock.h"

SpinLock::SpinLock() : m_use(0)
{
}

SpinLock::~SpinLock() 
{
}

void SpinLock::Enter()
{
	//��� ����
	while (true)
	{
		if (InterlockedExchange((LPLONG)&m_use, 1) == 0) break;
		Sleep(0);
	}
}

void SpinLock::Leave()
{
	InterlockedExchange((LPLONG)&m_use, 0);
}