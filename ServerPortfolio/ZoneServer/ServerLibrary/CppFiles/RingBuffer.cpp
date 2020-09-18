#include "../HeaderFiles/RingBuffer.h"

RingBuffer::RingBuffer(int _size, int _tempSize)
{
	buffer = nullptr;
	readPoint = nullptr;
	parsingPoint = nullptr;
	writePoint = nullptr;
	bufferStartPoint = nullptr;
	bufferMaxSize = 0;
	tempBufferSize = 0;
	dataInBuffer = 0;

	bufferMaxSize = _size;
	tempBufferSize = _tempSize;

	buffer = new char[_size];
	if (buffer == nullptr) return;

	bufferStartPoint = buffer + tempBufferSize;
	bufferEndPoint = buffer + bufferMaxSize;
	writePoint = readPoint = bufferStartPoint;
	parsingPoint = readPoint = bufferStartPoint;
}

RingBuffer::~RingBuffer()
{
	SAFE_DELETE_ARRAY(buffer);
}

void RingBuffer::Reset()
{
	bufferStartPoint = readPoint = parsingPoint = writePoint;
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
	//������ ���������� �д� ���������� ������
	int remainedSize = (int)(bufferEndPoint - readPoint);

	//������ ���������� �д� �������� ����� 1�ۿ� ���� ��� -> TempBuffer������ ������
	if (remainedSize == 1)
	{
		char* tempBuffer = bufferStartPoint - 1;
		*tempBuffer = *readPoint;
		readPoint = tempBuffer;
		//���� readPoint�� ��Ŷ �����ŭ �����ش�.
		readPoint += _size;

		return;
	}

	//������ ���������� �д� ������ ���� -> ������ ù�������� �д� ������ �Űܾ��Ѵ�.
	if (remainedSize == 0)
	{
		readPoint = bufferStartPoint;
	}

	//��Ŷ ����� �����ִ� ũ�⺸�� ũ��. -> ���� ©�ȴ�.
	if (_size > remainedSize)
	{
		//�� ������ �����Ѵ�.
		readPoint = (bufferStartPoint - remainedSize);
	}

	readPoint += _size;

	if (readPoint == bufferEndPoint)
	{
		readPoint = bufferStartPoint;
	}

	return;
}

char* RingBuffer::CanParsing()
{
	dataInBuffer = GetDataInBuffer();

	//������ �ȿ� �����Ͱ� 2�̻� �ִ°�? (��Ŷ ������ ���� �� �ִ� �ּ����� ũ�� = 2)
	if (dataInBuffer >= 2)
	{
		//������ �ȿ� PacketSize�� �о�� �� z�����Ƿ� �� �̻�ŭ �����ִ� �����Ͱ� �����Ѵٸ� Parsing();
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
	int remainedSize = (int)(bufferEndPoint - parsingPoint);
	//�Ľ��� �����ʹ� �д� �������� �о�´�.
	char* parsingData = parsingPoint;

	//������ ���������� �д� �������� ����� 1�ۿ� ���� ��� -> TempBuffer������ ������
	if (remainedSize == 1)
	{
		char* tempBuffer = bufferStartPoint - 1;
		*tempBuffer = *parsingPoint;
		parsingData = parsingPoint = tempBuffer;
		//���� parsingPoint�� ��Ŷ �����ŭ �����ش�.
		parsingPoint += GetPacketSize();

		return parsingData;
	}

	//������ ���������� �д� ������ ���� -> ������ ù�������� �д� ������ �Űܾ��Ѵ�.
	if (remainedSize == 0)
	{
		parsingPoint = bufferStartPoint;
	}

	WORD packetSize = GetPacketSize();

	//��Ŷ ����� �����ִ� ũ�⺸�� ũ��. -> ���� ©�ȴ�.
	if (packetSize > remainedSize)
	{
		//�� ������ �����Ѵ�.
		memcpy(bufferStartPoint - remainedSize, parsingPoint, remainedSize);
		parsingData = parsingPoint = (bufferStartPoint - remainedSize);
	}

	parsingPoint += packetSize;

	if (parsingPoint == bufferEndPoint)
	{
		parsingPoint = bufferStartPoint;
	}

	//�׽�Ʈ��
	/*if (this->GetDataInBuffer() > 0)
	{
		printf("%d \n", this->GetDataInBuffer());
	}*/

	return parsingData;
}

DWORD RingBuffer::GetWriteableSize()
{
	DWORD size;

	//����� ���
	if (readPoint < writePoint)
	{
		size = (DWORD)(bufferEndPoint - writePoint);
	}
	else if (readPoint == writePoint)
	{
		//readPoint�� writePoint�� ������
		//�����Ͱ� �ִ� ��� == �ѹ��� ���Ƽ� ������
		if (this->GetDataInBuffer() > 0)
		{
			size = 0;
		}
		//readPoint�� writePoint�� ������
		//�����Ͱ� ���� ��� == ringBuffer�� �����͸� �޴� �����̰ų� ó���Ұ� ����(����X)
		else
		{
			size = (DWORD)(bufferEndPoint - writePoint);
		}
	}
	else
	{
		size = (DWORD)(readPoint - writePoint);
	}

	return size;
}

DWORD RingBuffer::GetDataInBuffer()
{
	DWORD size;

	if (parsingPoint > writePoint)
	{
		size = (DWORD)(bufferEndPoint - parsingPoint + (writePoint - bufferStartPoint));
	}
	else
	{
		size = (DWORD)(writePoint - parsingPoint);
	}

	return size;
}

WORD RingBuffer::GetPacketSize()
{
	return *(WORD*)parsingPoint;
}