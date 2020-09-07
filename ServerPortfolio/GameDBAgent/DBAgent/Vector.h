#include <math.h>

//unity�� Vector�� �״�� �غ�

//������
//https://copycode.tistory.com/82

#pragma pack(push, 1)
struct VECTOR2
{
	float x;
	float y;

	VECTOR2()
	{
		x = 0;
		y = 0;
	}

	VECTOR2(float _x, float _y)
	{
		x = _x;
		y = _y;
	}

	VECTOR2 operator-(const VECTOR2& p2)
	{
		VECTOR2 vec;
		vec.x = x - p2.x;
		vec.y = y - p2.y;
		return vec;
	}

	VECTOR2 operator+(const VECTOR2& p2)
	{
		VECTOR2 vec;
		vec.x = x + p2.x;
		vec.y = y + p2.y;
		return vec;
	}

	VECTOR2 operator/(const float p2)
	{
		VECTOR2 vec;
		vec.x = x / p2;
		vec.y = y / p2;
		return vec;
	}

	VECTOR2 operator*(const float p2)
	{
		VECTOR2 vec;
		vec.x = x * p2;
		vec.y = y * p2;
		return vec;
	}

	//Unity3d
	//������ ���ؿ� ������ �Ÿ� ��
	//https://teddy.tistory.com/19

	float SqrMagnitude(void)
	{
		return x * x + y * y;
	}

	float Magnitude(void)
	{
		return sqrtf(x * x + y * y);
	}
};
#pragma pack(pop)   