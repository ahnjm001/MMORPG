#include "RingBuffer.h"

RingBuffer::RingBuffer()
{
	buffer = nullptr;
	readPoint = nullptr;
	writePoint = nullptr;
	bufferStartPoint = nullptr;
	bufferEndPoint = nullptr;
	bufferMaxSize = 0;
	tempBufferSize = 0;
	dataInBuffer = 0;
}

RingBuffer::~RingBuffer()
{

}

void RingBuffer::Init(int _size, int _tempSize)
{
	bufferMaxSize = _size;
	tempBufferSize = _tempSize;

	buffer = new char[_size];
	if (buffer == nullptr) return;

	bufferStartPoint = buffer + tempBufferSize;
	bufferEndPoint = buffer + bufferMaxSize;
	writePoint = readPoint = bufferStartPoint;
}

void RingBuffer::Reset()
{
	SAFE_DELETE_ARRAY(buffer);

	readPoint = nullptr;
	writePoint = nullptr;
	bufferStartPoint = nullptr;
	bufferEndPoint = nullptr;
	bufferMaxSize = 0;
	tempBufferSize = 0;
	dataInBuffer = 0;
}

void RingBuffer::Write(int _size)
{
	writePoint += _size;

	if (writePoint >= bufferEndPoint)
	{
		writePoint = bufferStartPoint;
	}
}

bool RingBuffer::CanParsing()
{
	dataInBuffer = GetDataInBuffer();

	//������ �ȿ� �����Ͱ� 2�̻� �ִ°�? (��Ŷ ������ ���� �� �ִ� �ּ����� ũ�� = 2)
	if (dataInBuffer >= 2)
	{
		return true;
	}

	//�����Ͱ� ���ų� 1���ͼ� ũ������ �� �� ���� ���
	return false;
}

char* RingBuffer::Parsing()
{
	//������ ���������� �д� ���������� ������
	int remainedSize = (int)(bufferEndPoint - readPoint);
	//�Ľ��� �����ʹ� �д� �������� �о�´�.
	char* parsingData = readPoint;

	//������ ���������� �д� �������� ����� 1�ۿ� ���� ��� -> �̾�����Ѵ�.
	if (remainedSize == 1)
	{
		char* tempBuffer = bufferStartPoint - 1;
		*tempBuffer = *readPoint;
		parsingData = readPoint = tempBuffer;

		readPoint += GetPacketSize();

		return parsingData;
	}
	//������ ���������� �д� ������ ���� = ������ ù�������� �д� ������ �Űܾ��Ѵ�.
	else if (remainedSize == 0)
	{
		readPoint = bufferStartPoint;
	}

	unsigned short packetSize = GetPacketSize();

	if (packetSize > remainedSize)
	{
		memcpy(bufferStartPoint - remainedSize, readPoint, remainedSize);
		parsingData = readPoint = (bufferStartPoint - remainedSize);
	}

	readPoint += packetSize;
	return parsingData;
}

int RingBuffer::GetWriteableSize()
{
	if (readPoint > writePoint)
	{
		return (int)(readPoint - writePoint);
	}
	else
	{
		return (int)(bufferEndPoint - writePoint);
	}
}

int RingBuffer::GetDataInBuffer()
{
	if (readPoint > writePoint)
	{
		return (bufferEndPoint - readPoint) + (writePoint - bufferStartPoint);
	}
	else
	{
		return (int)(writePoint - readPoint);
	}
}

unsigned short RingBuffer::GetPacketSize()
{
	return *(unsigned short*)readPoint;
}