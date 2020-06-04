#pragma once
#include "Field.h"

//=====================================================

//FieldManager

//Field���� �����ϱ����� �Ŵ���

//=====================================================

class FieldManager
{
private:
	map<WORD, Field*> m_fieldMap;
	int m_fieldCount;

public:
	FieldManager();
	~FieldManager();

	void Init();
	void InitMonsterThread();

	Field* GetField(int _num);

	int GetFieldCount() { return m_fieldCount; }
};