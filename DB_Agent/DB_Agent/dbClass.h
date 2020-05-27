#pragma once
#include <iostream>
#include <string>
#include <mysql.h>

#pragma comment(lib, "libmySQL.lib")

#pragma warning(disable:4996)

using namespace std;

//=====================================================

//MySQLClass

//MySQL�� �����ϴ� Ŭ����
//Connect & SelectDB�� rpg DB�� �����Ѵ�.
//���� �ٸ� �Լ���� �α���, ȸ������, ����DB������Ʈ, ����DB�ҷ����⸦ �����Ѵ�.

//=====================================================

class dbClass
{
private:
	//CriticalSectionClass	m_lock;

	const char*				m_dataName;

	MYSQL					connect;
	MYSQL_RES*				result;

	MYSQL_ROW				row;
	MYSQL_FIELD*			field;

	//���ڵ���� ����
	my_ulonglong			rowNum;
	//�ʵ��� ����
	unsigned int			fieldNum;

	//����� �ִ� �����ΰ�?
	bool					haveResultQuery;
	//������ �Ǿ� �ִ°�?
	bool					isConnect;

	//�̻��
	/*
	unsigned int			fieldNum;			//�ʵ��� ����
	my_ulonglong			currentPos;			//���� row �� offset

	bool					haveResultQuery;	//����� �ִ� �����ΰ���?
	bool					isBof;				//���ڵ���� ó���ΰ���?		(begin of file)
	bool					isEof;				//���ڵ���� �������ΰ���?	(end of file)
	*/

public:
	dbClass();
	~dbClass();

	void Init();
	bool Connect(const char* host,
		const char* username,
		const char* password,
		const char* database,
		const unsigned int port,
		const char* unix_socket,
		const unsigned int client_flag);
	void DisConnect();

	bool SelectDB(const char* DBname);

	//���̵� ���� ���� �ش� id�� �ִ��� üũ �� ��й�ȣ�� �´ٸ� �ش� ���� �ҷ��´�.
	bool OverlappedCheck(const char* id, const char* password, int &_userId);
	//���̵� ���� ���� �ش� id�� �ִ��� üũ �� �������� �ʴ´ٸ� �޾ƿ� ��й�ȣ�� ���� ȸ�������� ���ش�.
	bool InsertData(const char* id, const char* password);

	//�ش� ������ ��ȣ�� ������ ������ ã�Ƽ� �ҷ��´�.
	//_userId�� ���� ����Ǽ� �ȵȴ�. �׷��� const int&�� ���ڸ� �ҷ���.
	bool GetUserInfo(const int& _userId,
		char* _userName, int& _level, int& _curHp, int& _maxHp,
		int& _curMp, int& _maxMp, int& _curExp, int& _maxExp,
		int& _atk, int& _def);
	//�ش� ������ ��ȣ�� ������ ������ ã�� �� �࿡ ���� ���� ������ ������ DB�� ������Ʈ�Ѵ�.
	bool UpdateUserInfo(const int& _userId,
		int _level, int _curHp, int _maxHp,
		int _curMp, int _maxMp, int _curExp, int _maxExp,
		int _atk, int _def);

	//���� Ÿ�Կ� ���� ������ ������ �ҷ��´�.
	bool GetMonsterInfo(WORD& _monsterType, int& _curHp, int& _maxHp,
		float& _attackDelay, int& _attackDamage, float& _attackDistance,
		float& _moveSpeed, int& _patrolRange, float& _patrolDelay,
		float& returnDistance, int& _dropExp);

	void Query(const char* tableName);

	//�̻��
	/*
	void First();
	void Last();
	void Next();
	void Prev();
	void Move(int n);
	*/
};

