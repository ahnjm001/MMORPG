#include "HeartBeatThread.h"

HeartBeatThread::HeartBeatThread(SessionManager* _sessionManager)
{
	m_sessionManager = _sessionManager;

	Thread<HeartBeatThread>::Start(this);

	MYDEBUG("[ HeartBeatThread Init Success ]\n");
}


HeartBeatThread::~HeartBeatThread()
{
}

void HeartBeatThread::LoopRun()
{
	Packet* checkAlivePacket = new Packet();
	checkAlivePacket->Init(SendCommand::Zone2C_CHECK_ALIVE, sizeof(Packet));

	while (1)
	{
		HeartBeat(checkAlivePacket);

		Sleep(10000);
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

		//������ äŷ���� �ƴϰ�, ������ äŷ�� ���� �غ� �Ǿ��°�(���ӿ� Ȯ���� �����ߴ°�)
		//���忡 �������� ��� äŷ�� �غ�Ǿ��ٰ� ó���ߴ�.
		if (!user->IsChecking() && user->GetStartCheckingHeartBeat())
		{
			user->SetChecking(true);
			user->Send(reinterpret_cast<char*>(_packet), _packet->size);
		}
		//������ äŷ���̴�.(Ŭ���̾�Ʈ �ʿ��� HeartBeat Checking �Ϸ� ��Ŷ�� �ȿԴ�.)
		else if (user->IsChecking())
		{
			//���� ����
			user->SetConnected(false);

			MYDEBUG("[ HeartBeat Checking Failed ]\n");
		}
	});

	//�Ϲ����� For��
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