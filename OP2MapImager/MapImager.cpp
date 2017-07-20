#include "MapImager.h"
#include <iostream>

bool MapImager::imageMap(string& renderFilenameOut, const string& filename, const RenderSettings& renderSettings)
{
	bool saveGame = false;
	if (XFile::extensionMatches(filename, ".OP2"))
		saveGame = true;

	SeekableStreamReader* seekableStreamReader = resourceManager.getResourceStream(filename);

	if (seekableStreamReader == nullptr)
		throw std::exception("Unable to find specified map or save file.");

	MapData mapData(seekableStreamReader, saveGame);

	delete seekableStreamReader;

	RenderManager::Initialize();

	RenderManager mapImager(
		mapData.mapHeader.mapTileWidth(),
		mapData.mapHeader.mapTileHeight, 24, renderSettings.scaleFactor);

	loadTileSets(mapData, mapImager, renderSettings.accessArchives);
	setRenderTiles(mapData, mapImager);

	XFile::newDirectory(renderSettings.destDirectory);
	formatRenderFilename(renderFilenameOut, filename, renderSettings);

	bool saveSuccess = mapImager.SaveMapImage(renderFilenameOut, renderSettings.imageFormat);

	RenderManager::DeInitialize();

	if (!saveSuccess)
		cerr << "Error encountered when attempting to save " + renderFilenameOut << endl;

	return saveSuccess;
}

string MapImager::getImageFormatExtension(ImageFormat imageFormat)
{
	switch (imageFormat)
	{
	case ImageFormat::PNG:
		return ".png";
	case ImageFormat::BMP:
		return ".bmp";
	case ImageFormat::JPG:
		return ".jpg";
	default:
		return ".bmp";
	}
}

void MapImager::formatRenderFilename(string& renderFilenameOut, const string& filename, const RenderSettings& renderSettings)
{
	renderFilenameOut = XFile::appendSubDirectory(filename, renderSettings.destDirectory);
	string s = ".s" + to_string(renderSettings.scaleFactor);
	renderFilenameOut = XFile::appendToFilename(renderFilenameOut, s);
	renderFilenameOut = XFile::changeFileExtension(renderFilenameOut, getImageFormatExtension(renderSettings.imageFormat));

	if (!renderSettings.overwrite)
		renderFilenameOut = createUniqueFilename(renderFilenameOut);
}

string MapImager::createUniqueFilename(const string& filename)
{
	string uniqueFilename = filename;

	int pathIndex = 1;
	while (XFile::PathExists(uniqueFilename))
	{
		uniqueFilename = XFile::appendToFilename(filename, "_" + std::to_string(pathIndex));
		pathIndex++;

		//if (pathIndex >= std::numeric_limits<int>::max())
		if (pathIndex >= 32000)
			throw std::exception("Too many files with the same filename.");
	}

	return uniqueFilename;
}

void MapImager::loadTileSets(MapData& mapData, RenderManager& mapImager, bool accessArchives)
{
	for (size_t i = 0; i < mapData.tileSetSources.size(); ++i)
	{
		if (mapData.tileSetSources[i].numTiles == 0)
			continue;

		string tileSetFilename = mapData.tileSetSources[i].getTileSetFilename() + ".bmp";

		bool extracted = resourceManager.extractFromArchive(tileSetFilename);
		
		if (!extracted)
			throw std::exception(("Unable to find the tileset " + tileSetFilename + " in the directory or in a given archive (.vol).").c_str());

		mapImager.AddTileSet(tileSetFilename, ImageFormat::BMP);

		//TODO: Load Bmps into memory and set in mapImager if Outpost2 specific BMP file.
		//mapImager.AddTileSetRawBits()
	}
}

void MapImager::setRenderTiles(MapData& mapData, RenderManager& renderManager)
{
	for (unsigned int y = 0; y < mapData.mapHeader.mapTileHeight; y++)
		for (unsigned int x = 0; x < mapData.mapHeader.mapTileWidth(); x++)
			renderManager.PasteTile(mapData.getTileSetIndex(x, y), mapData.getImageIndex(x, y), x, y);
}