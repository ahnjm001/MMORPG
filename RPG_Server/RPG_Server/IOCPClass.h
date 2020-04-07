#pragma once
#include "stdafx.h"

#include "WorkerThread.h"
#include "CriticalSectionClass.h"

//������ �ϳ��� ���µ� �̱��� �Ẹ��
class IOCPClass
{
private:
	HANDLE			m_hIOCP;
	unsigned int	m_threadId;

	int				m_threadCount;
	WorkerThread**	m_workThreadBuffer;

	CriticalSectionClass m_locker;

public:
	IOCPClass();
	~IOCPClass();

	bool Init();
	void Reset();
	void AddSocket(SOCKET _socket, unsigned long long _value);
};