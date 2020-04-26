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

public:
	FieldManager();
	~FieldManager();

	void Init();

	Field* GetField(int _num);
};