#include "HeartBeatThread.h"



HeartBeatThread::HeartBeatThread()
{
}


HeartBeatThread::~HeartBeatThread()
{
}

void HeartBeatThread::Init(SessionManager* _sessionManager)
{
	m_sessionManager = _sessionManager;

	ThreadClass<HeartBeatThread>::Start(this);

	printf("[ HeartBeatThread Init Success ]\n");
}
	 
void HeartBeatThread::LoopRun()
{
	Packet* checkAlivePacket = new Packet();
	checkAlivePacket->Init(SendCommand::Zone2C_CHECK_ALIVE, sizeof(Packet));

	while (1)
	{
		HeartBeat(checkAlivePacket);

		Sleep(5000);
	}
}

void HeartBeatThread::HeartBeat(Packet* _packet)
{
	list<Session*>* vSessionList = m_sessionManager->GetItemList();
	
	list<Session*>::iterator iterBegin = vSessionList->begin();
	list<Session*>::iterator iterEnd = vSessionList->end();

	User* user;

	for_each(iterBegin, iterEnd, [=](Session* _session) mutable
	{
		user = dynamic_cast<User*>(_session);

		//유저가 채킹중이 아니고, 유저가 채킹에 대한 준비가 되었는가(게임에 확실히 접속했는가)
		//입장에 성공했을 경우 채킹에 준비되었다고 처리했다.
		if (!user->GetIsChecking() && user->GetStartCheckingHeartBeat())
		{
			user->SetIsChecking(true);
			user->Send(reinterpret_cast<char*>(_packet), _packet->size);
		}
		//유저가 채킹중이다.(클라이언트 쪽에서 HeartBeat Checking 완료 패킷이 안왔다.)
		else if (user->GetIsChecking())
		{
			//연결 끊기
			user->SetIsConnected(false);

			printf("[ HeartBeat Checking Failed ]\n");
		}
	});

	//일반적인 For문
	/*for (list<Session*>::iterator iter = vSessionList->begin(); iter != iterEnd;)
	{
		user = reinterpret_cast<User*>(*iter);
		++iter;

		if (!user->GetIsChecking() && user->GetStartCheckingHeartBeat())
		{
			user->SetIsChecking(true);
			user->Send(reinterpret_cast<char*>(_packet), _packet->size);
		}
		else if(user->GetIsChecking())
		{
			user->SetIsConnected(false);

			printf("[ HeartBeat Checking Failed ]\n");
		}
	}*/
}