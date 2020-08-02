#pragma once

#include "User.h"
#include "Monster.h"
#include "packet.h"
#include "UserManager.h"
#include "FieldManager.h"

class PacketHandler
{
private:
	UserManager&					m_userManager;
	FieldManager&					m_fieldManager;

public:
	PacketHandler(UserManager& _userManager, FieldManager& _fieldManager)
		: m_userManager(_userManager), m_fieldManager(_fieldManager)
	{
		userPacketFunc[0] = &PacketHandler::OnPacket_EnterField;				
		userPacketFunc[1] = &PacketHandler::OnPacket_EnterFieldSuccess;
		userPacketFunc[2] = &PacketHandler::OnPacket_EnterField;
		userPacketFunc[3] = &PacketHandler::OnPacket_EnterField;
		userPacketFunc[4] = &PacketHandler::OnPacket_UpdateUserPosition;		
		userPacketFunc[5] = &PacketHandler::OnPacket_UpdateUserPosition_Finish;	
		userPacketFunc[6] = &PacketHandler::OnPacket_UserAttackFailed;			
		userPacketFunc[7] = &PacketHandler::OnPacket_UserAttack;				
		userPacketFunc[8] = &PacketHandler::OnPacket_UserRevive;				
		userPacketFunc[9] = &PacketHandler::OnPacket_RegisterUser;				
		userPacketFunc[10] = &PacketHandler::OnPacket_LogInUser;					
		userPacketFunc[11] = &PacketHandler::OnPacket_RequireInfo;
		userPacketFunc[12] = &PacketHandler::OnPacket_UpdateUser;
		userPacketFunc[13] = &PacketHandler::OnPacket_ExitUser;
		userPacketFunc[14] = &PacketHandler::OnPacket_Chatting;
		userPacketFunc[15] = &PacketHandler::OnPacket_HeartBeat;
		userPacketFunc[16] = &PacketHandler::OnPacket_EnterTestUser;		
		userPacketFunc[17] = &PacketHandler::OnPacket_Chatting_Whisper;

		dbPacketFunc[0] = &PacketHandler::OnPacket_LogInSuccess;
		dbPacketFunc[1] = &PacketHandler::OnPacket_LogInFailed;
		dbPacketFunc[2] = &PacketHandler::OnPacket_LogInFailed;
		dbPacketFunc[3] = &PacketHandler::OnPacket_RegisterSuccess;
		dbPacketFunc[4] = &PacketHandler::OnPacket_RegisterFailed;
		dbPacketFunc[5] = &PacketHandler::OnPacket_GetUserInfoSuccess;
		dbPacketFunc[6] = &PacketHandler::OnPacket_GetUserInfoFailed;
		dbPacketFunc[7] = &PacketHandler::OnPacket_UpdateUserSuccess;
		dbPacketFunc[8] = &PacketHandler::OnPacket_UpdateUserFailed;
		dbPacketFunc[9] = &PacketHandler::OnPakcet_GetMonstersData;
	};
	~PacketHandler();

	//�������� ���� ��
	//2014 ���� ���
	/*typedef void (PacketHandler::*UserPacketFunc1)(User* _user, Packet* _packet);
	UserPacketFunc1  packFunc[100];*/

	//2014 ���� ���(c++11)
	using UserPacketFunction = void (PacketHandler::*)(User* _user, Packet* _packet);
	UserPacketFunction userPacketFunc[100];

	using DBConnectorPacketFunction = void (PacketHandler::*)(Packet* _packet);
	DBConnectorPacketFunction dbPacketFunc[100];

	void HandleUserPacket(User* _user, Packet* _packet);
	void HandleMonsterPacket(Monster* _monster, Packet* _packet);
	void HandleDBConnectorPacket(Packet* _packet);

	void OnPacket_EnterField(User* _user, Packet* _packet);
	void OnPacket_EnterFieldSuccess(User* _user, Packet* _packet);
	void OnPacket_UpdateUserPosition(User* _user, Packet* _packet);
	void OnPacket_UpdateUserPosition_Finish(User* _user, Packet* _packet);
	void OnPacket_UserAttackFailed(User* _user, Packet* _packet);
	void OnPacket_UserAttack(User* _user, Packet* _packet);
	void OnPacket_UserRevive(User* _user, Packet* _packet);
	void OnPacket_RegisterUser(User* _user, Packet* _packet);
	void OnPacket_LogInUser(User* _user, Packet* _packet);
	void OnPacket_RequireInfo(User* _user, Packet* _packet);
	void OnPacket_UpdateUser(User* _user, Packet* _packet);
	void OnPacket_ExitUser(User* _user, Packet* _packet);
	void OnPacket_Chatting(User* _user, Packet* _packet);
	void OnPacket_HeartBeat(User* _user, Packet* _packet);
	void OnPacket_EnterTestUser(User* _user, Packet* _packet);
	void OnPacket_Chatting_Whisper(User* _user, Packet* _packet);

	//Monster=====================================================
	void OnPacket_MonsterAttack(Monster* _monster, Packet* _packet);

	//DB agent====================================================
	void OnPacket_LogInSuccess(Packet* _packet);
	void OnPacket_LogInFailed(Packet* _packet);
	void OnPacket_RegisterSuccess(Packet* _packet);
	void OnPacket_RegisterFailed(Packet* _packet);
	void OnPacket_GetUserInfoSuccess(Packet* _packet);
	void OnPacket_GetUserInfoFailed(Packet* _packet);
	void OnPacket_UpdateUserSuccess(Packet* _packet);
	void OnPacket_UpdateUserFailed(Packet* _packet);
	void OnPakcet_GetMonstersData(Packet* _packet);

	User* FindUserAtTempList(SOCKET _socket);
	User* FindUserAtHashMap(SOCKET _socket);
};