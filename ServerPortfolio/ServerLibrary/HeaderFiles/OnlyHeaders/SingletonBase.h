#pragma once

//template <typename _CLASS >
//class CLSingleton
//{
//protected :
//	CLSingleton() {}
//
//public :
//	_CLASS& GetInstance(void)
//	{
//		static _CLASS	m_instance;
//		return m_instance;
//	}
//};

//template <typename T>
//class SingletonBase
//{
//protected:
//	static T* singleton;
//
//	SingletonBase() {};
//	~SingletonBase() {};
//
//public:
//	//�̱��� ������
//	static T* getSingleton(void);
//
//	//�̱��� ����
//	void releaseSingleton(void);
//};
//
////�̱��� �ʱ�ȭ
//template <typename T>
//T* SingletonBase<T>::singleton = nullptr;
//
////�̱��� ���� �Լ�
//template <typename T>
//T* SingletonBase<T>::getSingleton()
//{
//	//�̱����� �Ǿ������ʴٸ�
//	if (!singleton) singleton = new T;
//
//	return singleton;
//}
//
//template <typename T>
//void SingletonBase<T>::releaseSingleton()
//{
//	if (singleton)
//	{
//		delete singleton;
//
//		singleton = nullptr;
//	}
//}

template <typename _Class>
class SingletonBase
{
protected:
	SingletonBase() {};
	~SingletonBase() {};

public:
	//�̱��� ������
	static _Class* getSingleton()
	{	
		static _Class singleton;

		return &singleton;
	}
};