#include "HeartBeatThread.h"

HeartBeatThread::HeartBeatThread(LogInSessionManager& _sessionManager, int _cycle)
	: m_sessionManager(_sessionManager)
{
	m_cycle = _cycle * 1000;

	MYDEBUG("[ HeartBeat Init Success ]\n");

	Thread::Start(this);
}

HeartBeatThread::~HeartBeatThread()
{
}

void HeartBeatThread::LoopRun()
{
	while (1)
	{
		Sleep(m_cycle);

		MainThread::getSingleton()->HearBeatCheck();
	}
}

void HeartBeatThread::HeartBeat()
{
	const std::unordered_map<int, LogInSession*>& tempHashMap = m_sessionManager.GetSessionHashMap()->GetItemHashMap();

	LogInSession* session;

	m_end = std::chrono::high_resolution_clock::now();
	auto nowTime = std::chrono::high_resolution_clock::now();

	for (const auto& element : tempHashMap)
	{
		session = element.second;

		m_durationSec = std::chrono::duration_cast<std::chrono::seconds>(m_end - session->GetStartTime());

		if (m_durationSec.count() > 60/* && m_durationSec.count() < 10000.0f*/)
		{
			//���� ����
			session->DisConnect();

			MYDEBUG("[ HeartBeat Checking Failed ]\n");
		}
	}

	//for_each
	/*
	list<Session*>::const_iterator iterBegin = vSessionList.begin();
	list<Session*>::const_iterator iterEnd = vSessionList.end();

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
	});*/

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