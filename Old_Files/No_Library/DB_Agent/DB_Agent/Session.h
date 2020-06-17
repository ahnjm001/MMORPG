#pragma once

#pragma warning(disable:4996)

#include "stdafx.h"
#include "packet.h"

#include "RingBuffer.h"
#include "SendBuffer.h"
//#include "CircBuf.h"

//������ �α��� ���� ������ ���� ����Ѵ�.

//=====================================================

//Session

//User�� �⺻�� �Ǵ� Ŭ����

//=====================================================

//MMORPG ��������
//https://www.youtube.com/watch?v=yk-HD8YoyZg

//���Ӽ��������� ���̵�
//https://www.slideshare.net/devcatpublications/ndc2013-19986939

//����ü Ȯ��
//overlapped
struct ST_OVERLAPPED : public WSAOVERLAPPED
{
	class Session* session;
};

class Session
{
protected:
	SOCKET					m_socket;

	DWORD					m_recvBytes;
	DWORD					m_flags;
	DWORD					m_sendBytes;

	ST_OVERLAPPED			m_recvOverlapped;
	WSABUF					m_recvDataBuffer;

	ST_OVERLAPPED			m_sendOverlapped;
	WSABUF					m_sendDataBuffer;

	RingBuffer*				m_recvBuffer;
	SendBuffer*				m_sendBuffer;
	//CircBuf<Packet*>*		m_circBuffer;

	bool					m_isConnected;
	bool					m_isCheckingHeartBeat;
	bool					m_startCheckingHeartBeat;
	int						m_heartBeatCheckedCount;

private:

public:
	Session();
	~Session();

	virtual void Init();
	virtual void OnConnect(SOCKET _socket);
	virtual void Disconnect();
	virtual void Reset();
	virtual void HeartBeatChecked() {};

	//RECV SEND üũ��
	void CheckCompletion(bool _bool, ST_OVERLAPPED* _overlapped, DWORD _bytes);

	void Recv();
	void Send(Packet* _packet);
	void Send(char* _data, DWORD _bytes);
	void ReSend();

	virtual void Parsing() = 0;

	RingBuffer* GetRecvBuffer() { return m_recvBuffer; }
	SendBuffer* GetSendBuffer() { return m_sendBuffer; }
	//CircBuf<Packet*>* GetCircBuffer() { return m_circBuffer; }

	bool GetIsConnected() { return m_isConnected; }
	void SetIsConnected(bool _bool) { m_isConnected = _bool; }

	SOCKET GetSocket() { return m_socket; }

	bool GetIsChecking() { return m_isCheckingHeartBeat; }
	void SetIsChecking(bool _bool) { m_isCheckingHeartBeat = _bool; }

	void StartCheckingHeartBeat() { m_startCheckingHeartBeat = true; }
	bool GetStartCheckingHeartBeat() { return m_startCheckingHeartBeat; }
};