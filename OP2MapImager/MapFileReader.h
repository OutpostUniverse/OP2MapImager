#pragma once

#include "MapData.h"
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

class MapFileReader
{
public:
	void ReadMapData(MapData& mapData, string filename);

private:
	void ReadMapHeader(MapHeader& mapHeader, ifstream& file);
	void ReadTileData(MapData& mapData, ifstream& file);
	void ReadClipRect(MapData& mapData, ifstream& file);
	void ReadTileSetSources(MapData& mapData, ifstream& file);
	void ReadTileSetHeader(ifstream& file);
	void ReadTileInfo(MapData& mapData, ifstream& file);
};



void MapFileReader::ReadMapData(MapData& mapData, string filename)
{
	ifstream file(filename, ios::in | ios::binary);

	if (!file.is_open())
		throw exception("Map file could not be opened.");
	
	ReadMapHeader(mapData.mapHeader, file);
	ReadTileData(mapData, file);
	ReadClipRect(mapData, file);
	ReadTileSetSources(mapData, file);
	ReadTileSetHeader(file);
	ReadTileInfo(mapData, file);

	file.close();
}

void MapFileReader::ReadMapHeader(MapHeader& mapHeader, ifstream& file)
{
	file.read((char*)&mapHeader, sizeof(mapHeader));
}

void MapFileReader::ReadTileData(MapData& mapData, ifstream& file)
{
	mapData.tileDataVector.resize(mapData.mapHeader.mapTileHeight << mapData.mapHeader.lgMapTileWidth);
	file.read((char*)&mapData.tileDataVector[0], mapData.tileDataVector.size() * sizeof(TileData));
}

void MapFileReader::ReadClipRect(MapData& mapData, ifstream& file)
{
	file.read((char*)&mapData.clipRect, sizeof(mapData.clipRect));
}

void MapFileReader::ReadTileSetHeader(ifstream& file)
{
	char buffer[10];
	file.read(buffer, sizeof(buffer));
	if (strncmp(buffer, "TILE SET\x1a", sizeof(buffer)))
	{
		throw exception("'TILE SET' string not found.");
	}
}

void MapFileReader::ReadTileSetSources(MapData& mapData, ifstream& file)
{
	mapData.tileSetSources.resize((size_t)mapData.mapHeader.numTileSets);

	for (int i = 0; i < mapData.mapHeader.numTileSets; ++i)
	{
		size_t stringLength;
		file.read((char*)&stringLength, sizeof(stringLength));

		if (stringLength > 8)
			throw exception("Tile Set Name greater than 8 characters in length.");

		if (stringLength == 0)
			continue;

		file.read((char*)&mapData.tileSetSources[i].tileSetFilename, stringLength);

		file.read((char*)&mapData.tileSetSources[i].numTiles, sizeof(int));
	}
}

void MapFileReader::ReadTileInfo(MapData& mapData, ifstream& file)
{
	size_t numTileInfo;
	file.read((char*)&numTileInfo, sizeof(numTileInfo));
	mapData.tileInfoVector.resize(numTileInfo);
	file.read((char*)&mapData.tileInfoVector[0], numTileInfo * sizeof(TileInfo));
	size_t numTerrainTypes;
	file.read((char*)&numTerrainTypes, sizeof(numTerrainTypes));
	mapData.terrainTypeVector.resize(numTerrainTypes);
	file.read((char*)&mapData.terrainTypeVector[0], numTerrainTypes * sizeof(TerrainType));
}