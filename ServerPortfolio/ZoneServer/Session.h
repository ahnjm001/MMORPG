#pragma once
#include <winsock2.h>

#include "SendBuffer.h"

#include "Acceptor.h"
#include "Receiver.h"

//������ �α��� ���� ������ ���� ����Ѵ�.

//=====================================================

//Session

//User�� �⺻�� �Ǵ� Ŭ����

//=====================================================

//MMORPG ��������
//https://www.youtube.com/watch?v=yk-HD8YoyZg

//���Ӽ��������� ���̵�
//https://www.slideshare.net/devcatpublications/ndc2013-19986939

#define MYOVERLAPPED m_acceptor->GetOverlapped()

class Session
{
protected:
	int						m_index;
	SOCKET					m_socket;

	Acceptor*				m_acceptor;
	Receiver*				m_receiver;
	SendBuffer*				m_sendBuffer;

	bool					m_isConnected;

private:

public:
	Session();
	virtual ~Session();

	virtual void Init(SOCKET _listenSocket);
	virtual void OnConnect();
	virtual void Disconnect();
	virtual void Reset();
	//RECV SEND üũ��
	virtual void CheckCompletion(Acceptor::ST_OVERLAPPED* _overlapped) = 0;

	void StartAccept();
	void AssociateIOCP(HANDLE _handle);

	void Recv();
	void Send(char* _data, DWORD _bytes);
	void ReSend();

	SendBuffer* GetSendBuffer() { return m_sendBuffer; }

	Acceptor* GetAcceptor() { return m_acceptor; }

	int GetIndex() { return m_index; }

	SOCKET GetSocket() { return m_socket; }

	bool IsConnected() { return m_isConnected; }
	void SetConnected(bool _bool) { m_isConnected = _bool; }	
};