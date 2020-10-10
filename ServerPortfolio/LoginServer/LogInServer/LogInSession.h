#pragma once
#include <chrono>
#include "../../ZoneServer/ServerLibrary/HeaderFiles/ClientSession.h"

#include "packet.h"

class LogInSession : public ClientSession
{
public:
	bool m_isInHashMap;

	bool m_isAlreadyDisConnected;

private:
	int	m_idx;
	int m_zoneNum;

	int	m_heartBeatCheckedCount;
	std::chrono::high_resolution_clock::time_point	m_start;

public:
	LogInSession();
	~LogInSession();

	void OnConnect();
	void DisConnect();
	void Reset();

	void OnRecv();

	void PacketHandle(Packet* _packet);

	void LogInUser(LogInPacket* _packet);
	void RegisterUser(RegisterUserPacket* _packet);

	//����ִ��� üũ
	void HeartBeatChecked();

	//�α��� �ߺ���(�̹� ��������)->����ó��
	void LogInDuplicated();
	void LogInSuccess(int _num);
	void LogInFailed();

	void RegisterSuccess();
	void RegisterFailed();

	void ChangeZone(int _num);
	void SendDisconnect();

	int GetIdx() { return m_idx; }
	int GetZoneNum() { return m_zoneNum; }

	std::chrono::high_resolution_clock::time_point GetStartTime() { return m_start; }
};

