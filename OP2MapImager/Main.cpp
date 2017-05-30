#include "MapFileReader.h"
#include "MapData.h"
#include <string>
#include <iostream>

int main()
{
	MapFileReader mapReader;

	MapData mapData;
	mapReader.ReadMapData(mapData, "Ashes.map");

	cout << mapData.mapHeader.lgMapTileWidth << endl;
	cout << mapData.mapHeader.mapTileHeight << endl;
	cout << mapData.tileDataVector.size() << endl;
	cout << mapData.tileDataVector.at(0).tileIndex << endl;
	cout << mapData.tileDataVector.at(mapData.tileDataVector.size() - 1).tileIndex << endl;
	cout << mapData.clipRect.x1 << "," << mapData.clipRect.y1 << endl;
	cout << mapData.clipRect.x2 << "," << mapData.clipRect.y2 << endl;
	cout << mapData.tileSetSources[0].tileSetFilename << endl;
	cout << mapData.tileSetSources[0].GetTileSetFilename() << endl;
	cout << mapData.tileInfoVector.size() << endl;
	cout << mapData.terrainTypeVector.size() << endl;
	cout << mapData.terrainTypeVector[0].tileRange.start << endl;
	cout << mapData.terrainTypeVector[0].tileRange.end << endl;
	cout << mapData.terrainTypeVector[1].tileRange.start << endl;
	cout << mapData.terrainTypeVector[1].tileRange.end << endl;


	return 0;
}