#pragma once
#pragma warning(disable:4005)
#include "../HeaderFiles/Session.h"
#include "../HeaderFiles/Receiver.h"
#include "../HeaderFiles/Sender.h"
#include "../HeaderFiles/SharedPointer.h"
#include "../HeaderFiles/SpinLock.h"
#include <queue>	

class ClientSession : public Session
{
protected:
	Receiver*			m_receiver;
	Sender*				m_sender;

	bool				m_isTestClient;
	bool				m_isConnected;
	bool				m_isActived;

public:
	ClientSession();
	virtual ~ClientSession();

	//����
	void OnConnect();
	//���� ����
	virtual void DisConnect() override;
	//�ʱ�ȭ(��������)
	void Reset();
	virtual void HandleOverlappedIO(ST_OVERLAPPED* _overlapped) override;
	virtual void GenerateOverlappedIO(ST_OVERLAPPED* _overlapped) override;
	//�Ľ̿�
	virtual void OnRecv() = 0;

	//void Send(char* _data, DWORD _bytes);
	void AddToSendQueue(SharedPointer<char>& sharedptr);

	Receiver* GetReceiver() { return m_receiver; }
	Sender* GetSender() { return m_sender; }

	bool IsTestClient() { return m_isTestClient; }

	bool IsConnected() { return m_isConnected; }
	void SetConnected(bool _bConnected) { m_isConnected = _bConnected; }
};

