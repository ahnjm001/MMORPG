#pragma once
#include <list>

using namespace std;

enum TileType : unsigned short
{
	NON = 0, BLOCK = 1
};

enum DirectionType
{
	NW, N, NE, E, SE, S, SW, W
};

class Tile
{
private:
	unsigned short m_x;
	unsigned short m_y;

	TileType m_tileType;
	unsigned short m_spawnMonsterIndex;

	//���۳����� �� ������ ���� ���
	int m_goal;
	//�� ��忡�� ��ǥ���� ���� ����Ǵ� ���
	int m_heuristic;
	//goal�� heuristic�� ��. ���� ���� �ִ� ���
	int m_fitness;

	bool m_isClosed;
	bool m_isOpened;

	Tile* m_parentTile;
	Tile* m_neighborTiles[8];

public:
	Tile()
	{
		m_x = 0;
		m_y = 0;
		m_tileType = NON;

		m_goal = m_heuristic = m_fitness = 0;
		m_isClosed = m_isOpened = false;

		m_parentTile = nullptr;

		for (int i = 0; i < 8; i++)
		{
			m_neighborTiles[i] = nullptr;
		}

		m_spawnMonsterIndex = 0;
	}

	~Tile() {}

	void SetTile(unsigned short _x,
		unsigned short _y,
		TileType _tileType,
		unsigned short _spawnMonsterIndex)
	{
		m_x = _x;
		m_y = _y;
		m_tileType = _tileType;
		m_spawnMonsterIndex = _spawnMonsterIndex;
	}

	void Reset()
	{
		m_goal = m_heuristic = m_fitness = 0;
		m_isClosed = m_isOpened = false;

		m_parentTile = nullptr;
	}

	void SetNeighborTiles(DirectionType _dir, Tile* _tile)
	{
		m_neighborTiles[_dir] = _tile;
	}

	void SetNeighborParent(list<Tile*>* _openList, Tile * _finishTile)
	{
		//���� Ÿ�Ϸκ��� �ֺ� 8���� Ÿ�� üũ
		for (int i = 0; i < 8; i++)
		{
			//Ÿ���� �������� ������ continue
			if (m_neighborTiles[i] == nullptr) continue;

			//�ش� �ֺ� Ÿ���� �θ� ����
			SetParent(m_neighborTiles[i], _openList, _finishTile);
		}
	}

	void SetParent(Tile* _tile, list<Tile*>* _openList, Tile* _finishTile)
	{
		//�����ְų� ���� �ƴ� Ÿ���̸� return
		if (_tile->GetIsClosed() || _tile->m_tileType == BLOCK) return;

		//�ش� Ÿ���� �θ�� ���� Ÿ��
		_tile->m_parentTile = this;
		//���� Ÿ�ϱ����� F,G,H ���ϱ�
		_tile->PathScoring(this, _finishTile);

		//�����ִ��� üũ
		if (_tile->GetIsOpened()) return;

		//�������� �ʾҴٸ� OpenList�� �߰��ϰ� ���ȴٰ� üũ
		_openList->push_back(_tile);
		_tile->SetIsOpened(true);
	}

	void PathScoring(Tile* _currentTile, Tile* _finishTile)
	{
		SetGoal(_currentTile);
		SetHeuristic(_finishTile);
		SetFitness();
	}

	void SetGoal(Tile* _currentTile)
	{
		m_goal = (m_x == _currentTile->m_x || m_y == _currentTile->m_y ? 10 : 14);
	}
	void SetHeuristic(Tile* _finishTile)
	{
		m_heuristic = (abs(m_x - _finishTile->m_x) +
			abs(m_y - _finishTile->m_y)) * 10;
	}
	void SetFitness()
	{
		m_fitness = m_goal + m_heuristic;
	}

	//=================================================================

	void SetIsOpened(bool _bool)
	{
		m_isOpened = _bool;
	}

	void SetIsClosed(bool _bool)
	{
		m_isClosed = _bool;
	}

	bool GetIsOpened()
	{
		return m_isOpened;
	}

	bool GetIsClosed()
	{
		return m_isClosed;
	}

	Tile* GetParentTile()
	{
		return m_parentTile;
	}

	int GetFitness()
	{
		return m_fitness;
	}

	int GetX()
	{
		return m_x;
	}

	int GetY()
	{
		return m_y;
	}

	unsigned short GetSpawnMonsterIndex(void)
	{
		return m_spawnMonsterIndex;
	}

	TileType GetType()
	{
		return m_tileType;
	}
};

