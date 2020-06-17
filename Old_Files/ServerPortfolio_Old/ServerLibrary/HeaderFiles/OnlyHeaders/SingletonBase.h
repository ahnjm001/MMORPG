#pragma once

template <typename T>
class SingletonBase
{
protected:
	static T* singleton;

	SingletonBase() {};
	~SingletonBase() {};

public:
	//�̱��� ������
	static T* getSingleton(void);

	//�̱��� ����
	void releaseSingleton(void);
};

//�̱��� �ʱ�ȭ
template <typename T>
T* SingletonBase<T>::singleton = 0;

//�̱��� ���� �Լ�
template <typename T>
T* SingletonBase<T>::getSingleton()
{
	//�̱����� �Ǿ������ʴٸ�
	if (!singleton) singleton = new T;

	return singleton;
}

template <typename T>
void SingletonBase<T>::releaseSingleton()
{
	if (singleton)
	{
		delete singleton;

		singleton = 0;
	}
}