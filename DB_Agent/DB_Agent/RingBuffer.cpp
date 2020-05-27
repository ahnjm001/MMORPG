#include "RingBuffer.h"

RingBuffer::RingBuffer()
{
	buffer = nullptr;
	readPoint = nullptr;
	writePoint = nullptr;
	bufferStartPoint = nullptr;
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

	bufferStartPoint = readPoint = writePoint;
	dataInBuffer = 0;
}

void RingBuffer::Write(int _size)
{
	writePoint += _size;

	if (writePoint >= (bufferEndPoint))
	{
		writePoint = bufferStartPoint;
	}
}

void RingBuffer::Read(int _size)
{
	readPoint += _size;

	if (readPoint >= bufferEndPoint)
	{
		readPoint = bufferStartPoint;
	}
}

char* RingBuffer::CanParsing()
{
	dataInBuffer = GetDataInBuffer();

	//������ �ȿ� �����Ͱ� 2�̻� �ִ°�? (��Ŷ ������ ���� �� �ִ� �ּ����� ũ�� = 2)
	if (dataInBuffer >= 2)
	{
		//������ �ȿ� PacketSize�� �о�� �� �����Ƿ� �� �̻�ŭ �����ִ� �����Ͱ� �����Ѵٸ� Parsing();
		if (dataInBuffer >= GetPacketSize())
		{
			return Parsing();
		}

		//�����Ͱ� 2�̻������� ��Ŷ �����ŭ ����� ���� ���� ���
		//return nullptr;
	}

	//�����Ͱ� ���ų� 1���ͼ� ũ������ �� �� ���� ���
	return nullptr;
}

char* RingBuffer::Parsing()
{
	//������ ���������� �д� ���������� ������
	int remainedSize = (int)(bufferEndPoint - readPoint);
	//�Ľ��� �����ʹ� �д� �������� �о�´�.
	char* parsingData = readPoint;

	//������ ���������� �д� �������� ����� 1�ۿ� ���� ��� -> TempBuffer������ ������
	if (remainedSize == 1)
	{
		char* tempBuffer = bufferStartPoint - 1;
		*tempBuffer = *readPoint;
		parsingData = readPoint = tempBuffer;
		//���� readPoint�� ��Ŷ �����ŭ �����ش�.
		readPoint += GetPacketSize();

		return parsingData;
	}
	//������ ���������� �д� ������ ���� -> ������ ù�������� �д� ������ �Űܾ��Ѵ�.
	else if (remainedSize == 0)
	{
		readPoint = bufferStartPoint;
	}

	WORD packetSize = GetPacketSize();

	//��Ŷ ����� �����ִ� ũ�⺸�� ũ��. -> ���� ©�ȴ�.
	if (packetSize > remainedSize)
	{
		//�� ������ �����Ѵ�.
		memcpy(bufferStartPoint - remainedSize, readPoint, remainedSize);
		parsingData = readPoint = (bufferStartPoint - remainedSize);
	}

	readPoint += packetSize;

	//�׽�Ʈ��
	/*if (this->GetDataInBuffer() > 0)
	{
		printf("%d \n", this->GetDataInBuffer());
	}*/

	return parsingData;
}

DWORD RingBuffer::GetWriteableSize()
{
	//����� ���
	if (readPoint < writePoint)
	{
		return bufferEndPoint - writePoint;
	}
	else if (readPoint == writePoint)
	{
		//readPoint�� writePoint�� ������
		//�����Ͱ� �ִ� ��� == �ѹ��� ���Ƽ� ������
		if (this->GetDataInBuffer() > 0)
		{
			return 0;
		}
		//readPoint�� writePoint�� ������
		//�����Ͱ� ���� ��� == ringBuffer�� �����͸� �޴� �����̰ų� ó���Ұ� ����(����X)
		else
		{
			return bufferEndPoint - writePoint;
		}
	}
	else
	{
		return readPoint - writePoint;
	}
}

DWORD RingBuffer::GetReadableSize()
{
	if (readPoint <= writePoint)
	{
		return writePoint - readPoint;
	}
	else
	{
		return bufferEndPoint - readPoint;
	}

}

DWORD RingBuffer::GetDataInBuffer()
{
	if (readPoint > writePoint)
	{
		return bufferEndPoint - readPoint + (writePoint - bufferStartPoint);
	}
	else
	{
		return writePoint - readPoint;
	}
}

WORD RingBuffer::GetPacketSize()
{
	return *(WORD*)readPoint;
}