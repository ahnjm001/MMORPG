#pragma once
#include <iostream>

#define	SAFE_DELETE_ARRAY(p)	{ if (p) { delete[] (p); (p) = NULL;} }

//����̰� ������ ���ߴ� ������� ������ ���� ¥��
//�Ľ� �κ��� �� �� ���� ���� �ͺ��� źź��

//Init���� �� ���� ���̿� �ӽ� ���� ����� ����
//���۸� �޴ٰ� ������ �Ѿ�ԵǸ� ������ ��� ���
//��Ŷ�� �߷��� ��쿡 �� �κ� ��Ŷ�� memcpy����(1����Ʈ�� �����ǹ��� �󸶳� �߸��簣�� �߷����� �׳� memcpy�ϹǷ� ���� �߻� x)

//20191118
//�����۰� ����� ¥������ ���� ſ�� Ŭ��,������ ���� �Ҿ����ߴ�. ��ġ�ϱ� �ߵ�

class RingBuffer
{
private:
	//���� ������
	int			bufferMaxSize;
	//�ӽ� ���� ���� ������
	int			tempBufferSize;
	//���� �ּ�
	char*		buffer;
	//�д� ����
	char*		readPoint;
	//���� ����
	char*		writePoint;
	//������ ���� ����
	char*		bufferStartPoint;
	//������ �� ����
	char*		bufferEndPoint;
	//���� �� ������ ũ��
	int			dataInBuffer;

public:
	RingBuffer();
	~RingBuffer();

	//Initializing
	void Init(int _size, int _tempSize);
	//�ʱ�ȭ
	void Reset();
	//_size��ŭ �б�
	void Write(int _size);
	//�Ľ� �������� ���� üũ
	bool CanParsing();
	//�Ľ�
	char* Parsing();

	//�� �� �ִ� ������ ũ�� ���
	int GetWriteableSize();
	//������ �ȿ� �ִ� �������� �� ũ��
	int GetDataInBuffer();
	//��Ŷ ������ 
	unsigned short GetPacketSize();

	char* GetWritePoint() { return writePoint; }
	char* GetReadPoint() { return readPoint; }
	int GetBufferSize() { return bufferMaxSize; }
};

