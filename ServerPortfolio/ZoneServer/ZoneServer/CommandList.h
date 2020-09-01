#pragma once
#include <Windows.h>

//unsigned short
enum class RecvCommand : WORD
{
	C2Zone_TRY_ENTER_FIELD					= 0, //첫 시작때만 호출
	C2Zone_ENTER_FIELD_SUCCESS				= 1,
	C2Zone_ENTER_FIELD						= 2,
	C2Zone_ENTER_VILLAGE					= 3,
	C2Zone_USER_MOVE						= 4,
	C2Zone_USER_MOVE_FINISH					= 5,
	C2Zone_USER_ATTACK_FAILED				= 6,
	C2Zone_USER_ATTACK_MONSTER				= 7,
	C2Zone_USER_REVIVE						= 8,
	C2Zone_REGISTER_ACCOUNT					= 9,
	C2Zone_LOGIN							= 10,
	C2Zone_REQUIRE_INFO						= 11,
	C2Zone_UPDATE_INFO						= 12,
	C2Zone_EXIT_USER						= 13,
	C2Zone_CHATTING							= 14,
	C2Zone_CHECK_ALIVE						= 15,
	C2Zone_ENTER_TEST_USER					= 16,
	C2Zone_CHATTING_WHISPER					= 17,
	C2Zone_ENTER_TESTFIELD					= 18,

	DB2Zone_LOGIN_SUCCESS					= 100,
	DB2Zone_LOGIN_FAILED_INVALID_ID			= 101,
	DB2Zone_LOGIN_FAILED_WRONG_PASSWORD		= 102,
	DB2Zone_REGISTER_SUCCESS				= 103,
	DB2Zone_REGISTER_FAILED					= 104,
	DB2Zone_GET_USER_DATA_SUCCESS			= 105,
	DB2Zone_GET_USER_DATA_FAILED			= 106,
	DB2Zone_UPDATE_USER_SUCCESS				= 107,
	DB2Zone_UPDATE_USER_FAILED				= 108,
	DB2Zone_MONSTERS_DATA					= 109,
	DB2Zone_ALIVE							= 110,

	LogIn2Zone_HELLO						= 200,
	LogIn2Zone_DISCONNECT_USER				= 201,
	Login2Zone_ALIVE						= 202,
	Login2Zone_AUTHENTICATION_SUCCESS		= 203,
	Login2Zone_AUTHENTICATION_FAILED		= 204,
	Login2Zone_DISCONNECT_USER_CHANGE_ZONE	= 205,

	Path2Zone_PATHFIND_SUCCESS				= 500,
	Path2Zone_PATHFIND_FAILED				= 501
};

enum class SendCommand : WORD
{
	Zone2C_USER_MOVE = 4,
	Zone2C_USER_MOVE_FINISH = 5,
	Zone2C_USER_ATTACK_FAILED = 6,
	Zone2C_USER_ATTACK_MONSTER = 7,
	Zone2C_CHATTING = 14,
	Zone2C_CHATTING_WHISPER = 17,
	Zone2C_CHANGE_ZONE = 18,

	Zone2LogIn_HELLO = 300,
	Zone2LogIn_DISCONNECT_USER = 301,
	Zone2Login_HEARTBEAT = 302,
	Zone2Login_AUTHENTICATION = 303,

	Zone2Path_PATHFIND = 400,
	Zone2Path_HEARTBEAT = 401,

	Zone2C_CHATTING_WHISPER_FAIL = 500,

	LogIn2C_ISCONNECTED = 1000,
	Zone2C_ISCONNECTED = 1001,

	Zone2C_REGISTER_USER = 1002,

	Zone2C_ENTER_FIELD = 1004,
	Zone2C_USER_LIST = 1005,
	Zone2C_USER_LIST_IN_RANGE = 1006,
	Zone2C_USER_LIST_VISIBLE = 1007,
	Zone2C_USER_LIST_INVISIBLE = 1008,
	Zone2C_ENTER_USER_INFO = 1009,
	Zone2C_ENTER_USER_INFO_IN_RANGE = 1010,
	Zone2C_LEAVE_USER_INFO = 1011,
	Zone2C_USER_HIT = 1012,
	Zone2C_USER_PLUS_EXP = 1013,
	Zone2C_USER_LEVEL_UP = 1014,
	Zone2C_USER_DEATH = 1015,
	Zone2C_USER_REVIVE = 1016,
	Zone2C_LEAVE_SECTOR_USER_INFO = 1017,
	Zone2C_ENTER_SECTOR_USER_INFO = 1018,

	Zone2C_MONSTER_INFO_LIST = 1019,
	Zone2C_MONSTER_INFO_LIST_IN_RANGE = 1020,
	Zone2C_MONSTER_INFO_LIST_VISIBLE = 1021,
	Zone2C_MONSTER_INFO_LIST_INVISIBLE = 1022,
	Zone2C_MONSTER_INFO = 1023,

	Zone2C_MONSTER_MOVE = 1024,
	Zone2C_MONSTER_ATTACK = 1025,
	Zone2C_MONSTER_HIT = 1026,
	Zone2C_MONSTER_HIT_FAIL = 1027,

	Zone2C_CHECK_ALIVE = 1028,

	Zone2C_REGISTER_USER_SUCCESS = 1029,
	Zone2C_REGISTER_USER_FAILED = 1030,

	Zone2C_LOGIN_SUCCESS = 1031,
	Zone2C_LOGIN_FAILED = 1032,
	Zone2C_LOGIN_FAILED_DUPLICATED = 1033,

	Zone2DB_LOGIN = 1034,
	Zone2DB_REGISTER = 1035,
	Zone2DB_REQUEST_USER_DATA = 1036,
	Zone2DB_REQUEST_MONSTER_DATA = 1037,
	Zone2DB_UPDATE_USER = 1038,

	Zone2C_REGISTER_TEST_USER = 1039,
	Zone2C_UPDATE_STATE_TEST_USER = 1040,
	
	Zone2DB_HEARTBEAT = 1042
};