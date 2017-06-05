#include "MapFileReader.h"
#include "MapData.h"
#include "FreeImage/FreeImage.h"
#include <string>
#include <iostream>

/** Generic image loader
@param filename Pointer to the full file name
@param flag Optional load flag constant
@return Returns the loaded dib if successful, returns NULL otherwise
*/
FIBITMAP* BmpLoader(const char* filename, int flag) {
	return FreeImage_Load(FREE_IMAGE_FORMAT::FIF_BMP, filename, flag);
}

void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message) {
	printf("\n*** ");
	if (fif != FIF_UNKNOWN) {
		printf("%s Format\n", FreeImage_GetFormatFromFIF(fif));
	}
	printf(message);
	printf(" ***\n");
}

int main()
{
	MapFileReader mapReader;

	MapData mapData;
	mapReader.ReadMapData(mapData, "Ashes.map");

	FreeImage_Initialise();
	FreeImage_SetOutputMessage(FreeImageErrorHandler);

	std::vector<FIBITMAP*> tileSetImages;
	tileSetImages.resize(mapData.tileSetSources.size());

	for (size_t i = 0; i < mapData.tileSetSources.size(); ++i)
	{
		int flag = 0;
		std::string filename = mapData.tileSetSources[i].GetTileSetFilename() + ".bmp";
		tileSetImages[i] = FreeImage_Load(FREE_IMAGE_FORMAT::FIF_BMP, filename.c_str(), flag);
	}

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

	cout << mapData.GetTileIndex(0, 0) << endl;
	cout << mapData.GetTileIndex(1, 0) << endl;
	cout << mapData.GetTileIndex(0, 1) << endl;
	cout << mapData.GetTileIndex(32, 0) << endl;

	cout << FreeImage_GetColorsUsed(tileSetImages[0]) << endl;

	bool bSuccess = FreeImage_Save(FREE_IMAGE_FORMAT::FIF_PNG, tileSetImages[0], (mapData.tileSetSources[0].GetTileSetFilename() + ".png").c_str(), PNG_DEFAULT);

	FreeImage_DeInitialise();

	return 0;
}