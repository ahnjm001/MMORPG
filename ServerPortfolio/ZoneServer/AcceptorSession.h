#pragma once
#include "Session.h"

#include "../ServerLibrary/HeaderFiles/OnlyHeaders/IpEndPoint.h"

#include <winsock2.h>
#include <MSWSock.h>

#pragma comment(lib, "mswsock.lib")

#define	SAFE_DELETE_ARRAY(p)	{ if (p) { delete[] (p); (p) = NULL;} }

//�̸� �ٲٱ�
class AcceptorSession : public Session
{
private:
	int						m_tempNum;

	HANDLE					m_hIOCP;
	IpEndPoint				m_ipEndPoint;
	//�ۿ��� �����ؼ� ��������
	SOCKET					m_listenSocket;

	char*					m_byteBuffer;
	int						m_locallen;
	int						m_remotelen;
	sockaddr_in*			m_plocal;
	sockaddr_in*			m_premote;
		
public:
	AcceptorSession(SOCKET _listenSocket, IpEndPoint* _ipEndPoint, HANDLE _handle, int _num);
	~AcceptorSession();

	void Reset();

	void HandleOverlappedIO(ST_OVERLAPPED* _overlapped) override;
	void DisConnect();

	bool ActuateAcceptEx();

	SOCKET GetListenSocket() { return m_listenSocket; }
};

