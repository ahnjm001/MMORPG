#pragma once
#include <winsock2.h>
#include <WS2tcpip.h>
#include <process.h>
#include <stdexcept>

#pragma comment(lib, "ws2_32.lib")

template<typename T>
class EventClass
{
private:
	HANDLE hEvent;
	HANDLE hThread;
	unsigned int threadID;

public:
	EventClass()
	{
		hEvent = 0;
	}

	virtual ~EventClass()
	{
		if (hEvent)
		{
			CloseHandle(hEvent);
			hEvent = 0;
		}
	}

	void CreateEventHandle(T* _obj)
	{
		hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		hThread = (HANDLE)_beginthreadex(NULL, 0, &ThreadFunc, (void*)_obj, 0, &threadID);

		//�ٱ���
		//WaitForSingleObject(hEvent, INFINITE);
	}

	//EventFunc�� ����� �Լ�
	virtual void EventFunc(void) = 0;

	//_beginthreadex()�� �� static �Լ�.
	static unsigned int __stdcall ThreadFunc(void* pArgs)
	{
		T* obj = (T*)pArgs;
		obj->EventFunc();
		return 0;
	}

	HANDLE GetEventHandle() { return hEvent; }
	HANDLE GetThreadHandle() { return hThread; }
};