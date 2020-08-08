#pragma once
#define WIN32_LEAN_AND_MEAN

//#define _DEFINE_ODBC_

#include <tchar.h>
#include <iostream>
#include <string>

#include <mysql.h>
#include <sql.h>
#include <sqlext.h>

#include "packet.h"

#include "../ServerLibrary/HeaderFiles/SendBuffer.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/Thread.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/DoubleQueue.h"

#pragma comment(lib, "libmySQL.lib")

#pragma warning(disable:4996)

enum CONNECTOR_STATE
{
	READY,
	ACTIVE
};

class DBConnector : public Thread<DBConnector>
{
private:
	HANDLE					m_hEvent;

	CONNECTOR_STATE			m_state;
	bool					m_isConnect;

#ifdef _DEFINE_ODBC_
	SQLHENV					m_henv;
	SQLHDBC					m_hdbc;
	SQLHSTMT				m_hstmt;
	SQLRETURN				m_retcode;
#else
	MYSQL					m_connect;
	MYSQL_RES*				m_result;

	MYSQL_ROW				row;
	MYSQL_FIELD*			field;

	//���ڵ���� ����
	my_ulonglong			rowNum;
	//�ʵ��� ����
	unsigned int			fieldNum;
#endif						
	int						m_num;

	Packet*					m_packet;

	SendBuffer*				m_sendBuffer;

public:
	DBConnector(int _num);
	~DBConnector();

	bool Connect(const char* host,
		const char* username,
		const char* password,
		const char* database,
		const unsigned int port,
		const char* unix_socket,
		const unsigned int client_flag);
	void DisConnect();

#ifdef _DEFINE_ODBC_
	bool Query(const char* query);
#else
	bool SelectDB(const char* DBname);
	MYSQL_RES* Query(const char* query);
#endif	

	void Login(LogInPacket_DBAgent* _packet);
	void Register(RegisterPacket_DBAgent* _packet);
	void GetUserInfo(RequireUserInfoPacket_DBAgent* _packet);
	void UpdateUser(UpdateUserPacket* _packet);
	void GetMonsterInfo();

	void SetPacket(Packet* _packet);

	void LoopRun();

	int GetNum() { return m_num; }
	CONNECTOR_STATE GetState() { return m_state; }
};