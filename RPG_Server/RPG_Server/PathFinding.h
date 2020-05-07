#pragma once
#include "stdafx.h"
#include "Tile.h"

class PathFinding
{
public:
	void PathFind(list<VECTOR2>* _pathList, Tile* _startTile, Tile* _finishTile)
	{
		//openList = �ֺ��� �� �� �ִ� ����� ���� List
		//clostList = ���¸���Ʈ���� F�� ���� ���� ���� Tile���� ���� ����Ʈ (�� �� �ִ� ��)

		//�ʱ�ȭ �� openList, closeList ����
		_pathList->clear();
		list<Tile*> openList;
		list<Tile*> closeList;

		//����Ÿ���� �ʱ�ȭ�ϰ� �����ٰ� üũ
		_startTile->Reset();
		openList.push_back(_startTile);
		_startTile->SetIsOpened(true);

		_startTile->SetNeighborParent(&openList, _finishTile);

		//����Ÿ���� OpenList���� ���� �� CloseList�� �߰� �� �ݾҴٰ� üũ
		openList.pop_front();
		_startTile->SetIsOpened(false);

		closeList.push_back(_startTile);
		_startTile->SetIsClosed(true);

		Tile* currentTile = nullptr;

		//closeList�� _startTile���� _finishTile������ �ִܰ�θ� ���� ���������� �ݺ��Ǵ� �ݺ���
		while (1)
		{
			//openList�� IsTileChecked �������� ����
			//https://twpower.github.io/71-use-sort-and-stable_sort-in-cpp
			openList.sort(IsTileChecked);

			currentTile = openList.front();

			if (currentTile == _finishTile)
			{
				closeList.push_back(currentTile);
				currentTile->SetIsClosed(true);

				break;
			}

			if (!currentTile->GetIsClosed())
			{
				currentTile->SetNeighborParent(&openList, _finishTile);

				closeList.push_back(currentTile);
				openList.pop_front();

				currentTile->SetIsOpened(false);
				currentTile->SetIsClosed(true);
			}
		}

		//closeList�� �������� _pathList�� ���� ��´�.
		//(������ currentTile�� _finishTile�� ����Ű�� �ֱ⶧���� �״�� �ٽ� �������� _pathList�� ������ȴ�.)
		while (1)
		{
			_pathList->push_back(VECTOR2(static_cast<float>(currentTile->GetX()), 
				static_cast<float>(currentTile->GetY())));

			currentTile = currentTile->GetParentTile();

			if (currentTile == nullptr) break;
		}

		//�������� ��������Ƿ� reverse���ش�.
		reverse(_pathList->begin(), _pathList->end());
		//_pathList�� ���� �պκ��� _startTile�̹Ƿ� �����Ѵ�.
		//(���� �� �ִ� ������ _startTile�̹Ƿ� �̵��ϴµ� �ʿ����.)
		_pathList->pop_front();
		//_pathList.erase(_pathList.begin());

		//���ݱ��� ��ã�⿡ ���ƴ� Ÿ�ϵ� �ʱ�ȭ�Ѵ�.
		list<Tile*>::iterator openIterEnd = openList.end();
		list<Tile*>::iterator closeIterEnd = closeList.end();
		Tile* tile;

		for (list<Tile*>::iterator iter = openList.begin(); iter != openIterEnd; iter++)
		{
			tile = *iter;
			tile->SetIsOpened(false);
		}

		for (list<Tile*>::iterator iter = closeList.begin(); iter != closeIterEnd; iter++)
		{
			tile = *iter;
			tile->SetIsClosed(false);
		}
	}

	static bool IsTileChecked(Tile* a, Tile* b)
	{
		return (a->GetFitness() < b->GetFitness());
	}
};