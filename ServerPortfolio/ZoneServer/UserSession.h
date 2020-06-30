#pragma once
#include "Session.h"
#include "Receiver.h"
#include "SendBuffer.h"

class UserSession : public Session
{
protected:
	Receiver*			m_receiver;
	SendBuffer*			m_sendBuffer;

	bool				m_isTestClient;
	bool				m_isConnected;

	bool				m_ConstructorBool;

public:
	UserSession();
	virtual ~UserSession();

	//����
	void OnConnect();
	//���� ����
	void DisConnect();
	//�ʱ�ȭ(��������)
	void Reset();
	//�Ľ̿�
	virtual void Parsing() = 0;

	void Recv();
	void Send(char* _data, DWORD _bytes);

	SendBuffer* GetSendBuffer() { return m_sendBuffer; }

	bool IsTestClient() { return m_isTestClient; }

	bool IsConnected() { return m_isConnected; }
	void SetConnected(bool _bConnected) { m_isConnected = _bConnected; }
};

