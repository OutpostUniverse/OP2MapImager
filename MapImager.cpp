#include "MapImager.h"
#include <iostream>
#include <memory>
#include <stdexcept>

using namespace std;

bool MapImager::imageMap(string& renderFilenameOut, const string& filename, const RenderSettings& renderSettings)
{
	bool saveGame = isSavedGame(filename);

	MapData mapData(resourceManager.getResourceStream(filename, renderSettings.accessArchives), saveGame);

	RenderManager::initialize();

	RenderManager mapImager(
		mapData.mapHeader.mapTileWidth(),
		mapData.mapHeader.mapTileHeight, 24, renderSettings.scaleFactor);

	loadTileSets(mapData, mapImager, renderSettings.accessArchives);
	setRenderTiles(mapData, mapImager);

	XFile::createDirectory(renderSettings.destDirectory);
	renderFilenameOut = formatRenderFilename(filename, renderSettings);

	bool saveSuccess = mapImager.saveMapImage(renderFilenameOut, renderSettings.imageFormat);

	RenderManager::deInitialize();

	if (!saveSuccess)
		cerr << "Error encountered when attempting to save " + renderFilenameOut << endl << endl;

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

string MapImager::formatRenderFilename(const string& filename, const RenderSettings& renderSettings)
{
	string renderFilename;

	if (XFile::isRootPath(renderSettings.destDirectory))
		renderFilename = XFile::replaceFilename(renderSettings.destDirectory, filename);
	else
		renderFilename = XFile::appendSubDirectory(XFile::getFilename(filename), renderSettings.destDirectory);

	string s = ".s" + to_string(renderSettings.scaleFactor);
	renderFilename = XFile::appendToFilename(renderFilename, s);
	renderFilename = XFile::changeFileExtension(renderFilename, getImageFormatExtension(renderSettings.imageFormat));

	if (!renderSettings.overwrite)
		renderFilename = createUniqueFilename(renderFilename);

	return renderFilename;
}

string MapImager::createUniqueFilename(const string& filename)
{
	string uniqueFilename = filename;

	int pathIndex = 1;
	while (XFile::pathExists(uniqueFilename))
	{
		uniqueFilename = XFile::appendToFilename(filename, "_" + std::to_string(pathIndex));
		pathIndex++;

		if (pathIndex >= 32000)
			throw runtime_error("Too many files with the same filename.");
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

		//TODO: Allow FreeImage to take a pointer to the associated well within a vol file 
		//      instead of forcing its extraction.
		bool extracted = resourceManager.extractSpecificFile(tileSetFilename);
		
		if (!extracted)
			throw runtime_error("Unable to find the tileset " + tileSetFilename + " in the directory or in a given archive (.vol).");

		mapImager.addTileSet(tileSetFilename, ImageFormat::BMP);

		//TODO: If tilesets are Outpost 2 specific, translate the Outpost 2 specific tilesets into 
		//      standard bmp files for use in rendering map.
		//mapImager.addTileSetRawBits()
	}
}

void MapImager::setRenderTiles(MapData& mapData, RenderManager& renderManager)
{
	for (unsigned int y = 0; y < mapData.mapHeader.mapTileHeight; y++)
		for (unsigned int x = 0; x < mapData.mapHeader.mapTileWidth(); x++)
			renderManager.pasteTile(mapData.getTileSetIndex(x, y), mapData.getImageIndex(x, y), x, y);
}

bool MapImager::isSavedGame(string filename)
{
	return XFile::extensionMatches(filename, ".OP2");
}
