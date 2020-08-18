#pragma once
#include <Windows.h>
#include <iostream>

#define	SAFE_DELETE_ARRAY(p)	{ if (p) { delete[] (p); (p) = NULL;} }

//���� (Ȯ���� ���ذ� �ȵǼ� ũ�� ����� �ȵ�)
//https://github.com/jacking75/NetworkProg_Projects/tree/87e98bc00a5c3383281e3ca6178c0f6b4ce51a69
//nhn->iocpnetlib->circularBuffer.h

//=====================================================

//RingBuffer

//���������̴�. �� ���� ũ��� Init���� �����ش�. �׸��� ������ �̾���̱�� TempBuffer�� �д�.
//�� ������ ũ��� ������ ������ ������� ������ ũ��� �ٸ���.
//���۸� ��� �̾�ٿ����� �޴ٰ� �� ���� ũ�⸦ �Ѿ�ԵǸ� �� �պκ����� ���ư��� �ٽ� �޴´�.
//��Ŷ�� �������� �� �պκа� �� �޺κ����� ©���� �����ԵǸ� ringBuffer�� �պκ����� ������ �ٿ��ش�.
//(��Ŷ�� �߸��� Ȯ�εǸ� ������ ���̹Ƿ� �����Ʈ�� �߸����� �߿����� �ʴ�.)

/*
						  readPoint        writePoint
							 ��			       ��
				 �ߡߡߡߡߡߡߡޡޡޡޡޡڡڡڡڡ�				�� = ringBuffer�� ũ�⸦ �Ѿ�� ��Ŷ�� �߸��� �κ�
							��
	readPoint        writePoint
	   ��				 ��
		�ڡڡڡڡڡߡߡߡߡߡߡߡߡߡߡߡ�
*/

//=====================================================

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
	//�Ľ� ����
	char*		parsingPoint;
	//���� ����
	char*		writePoint;
	//������ ���� ����
	char*		bufferStartPoint;
	//������ �� ����
	char*		bufferEndPoint;
	//���� �� ������ ũ��
	int			dataInBuffer;

public:
	//RingBuffer(const RingBuffer&) = delete;
	RingBuffer(int _size, int _tempSize);
	~RingBuffer();

	//�ʱ�ȭ
	void Reset();
	//_size��ŭ ����,�б�
	void Write(int _size);
	void MoveParsingPoint(int _size);
	void Read(int _size);
	//�Ľ� �������� ���� üũ
	char* CanParsing();
	//�Ľ�
	char* Parsing();

	//�� �� �ִ�, ���� �� �ִ� ������ ũ�� ���
	DWORD GetWriteableSize();
	DWORD GetReadableSize();
	//������ �ȿ� �ִ� �������� �� ũ��
	DWORD GetDataInBuffer();
	//��Ŷ ������ 
	WORD GetPacketSize();

	char* GetWritePoint() { return writePoint; }
	char* GetReadPoint() { return readPoint; }
	int GetBufferSize() { return bufferMaxSize; }
};

