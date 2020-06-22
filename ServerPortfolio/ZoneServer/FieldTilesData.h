#pragma once
#include <iostream>
#include <fstream>

#include "Tile.h"

using namespace std;

//=====================================================

//FieldTilesData

//Field�� Ÿ�� ������ ������ �ִ� Ŭ����

//=====================================================

class FieldTilesData
{
private:
	int m_mapSizeX;
	int m_mapSizeY;

	Tile** m_Map;

public:
	FieldTilesData();
	~FieldTilesData();

	bool GetMap(const char * _name);
	Tile** GetMap();
	int GetMapSizeX();
	int GetMapSizeY();
	Tile* GetTile(int _x, int _y);
	//Tile** GetTiles(int _x, int _y);
};

