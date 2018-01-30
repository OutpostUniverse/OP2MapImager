#include "MapImager.h"
#include <iostream>
#include <memory>
#include <stdexcept>

using namespace std;

bool MapImager::ImageMap(string& renderFilenameOut, const string& filename, const RenderSettings& renderSettings)
{
	bool saveGame = IsSavedGame(filename);

	MapData mapData(resourceManager.GetResourceStream(filename, renderSettings.accessArchives), saveGame);

	RenderManager::Initialize();

	RenderManager mapImager(
		mapData.mapHeader.mapTileWidth(),
		mapData.mapHeader.mapTileHeight, 24, renderSettings.scaleFactor);

	LoadTileSets(mapData, mapImager, renderSettings.accessArchives);
	SetRenderTiles(mapData, mapImager);

	XFile::NewDirectory(renderSettings.destDirectory);
	renderFilenameOut = FormatRenderFilename(filename, renderSettings);

	bool saveSuccess = mapImager.SaveMapImage(renderFilenameOut, renderSettings.imageFormat);

	RenderManager::Deinitialize();

	if (!saveSuccess) {
		cerr << "Error encountered when attempting to save " + renderFilenameOut << endl << endl;
	}

	return saveSuccess;
}

string MapImager::GetImageFormatExtension(ImageFormat imageFormat)
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

string MapImager::FormatRenderFilename(const string& filename, const RenderSettings& renderSettings)
{
	string renderFilename;

	if (XFile::IsRootPath(renderSettings.destDirectory)) {
		renderFilename = XFile::ReplaceFilename(renderSettings.destDirectory, filename);
	}
	else {
		renderFilename = XFile::AppendSubDirectory(XFile::GetFilename(filename), renderSettings.destDirectory);
	}

	string s = ".s" + to_string(renderSettings.scaleFactor);
	renderFilename = XFile::AppendToFilename(renderFilename, s);
	renderFilename = XFile::ChangeFileExtension(renderFilename, GetImageFormatExtension(renderSettings.imageFormat));

	if (!renderSettings.overwrite) {
		renderFilename = CreateUniqueFilename(renderFilename);
	}

	return renderFilename;
}

string MapImager::CreateUniqueFilename(const string& filename)
{
	string uniqueFilename = filename;

	int pathIndex = 1;
	while (XFile::PathExists(uniqueFilename))
	{
		uniqueFilename = XFile::AppendToFilename(filename, "_" + std::to_string(pathIndex));
		pathIndex++;

		if (pathIndex >= 32000) {
			throw runtime_error("Too many files with the same filename.");
		}
	}

	return uniqueFilename;
}

void MapImager::LoadTileSets(MapData& mapData, RenderManager& mapImager, bool accessArchives)
{
	for (size_t i = 0; i < mapData.tileSetSources.size(); ++i)
	{
		if (mapData.tileSetSources[i].numTiles == 0) {
			continue;
		}

		string tileSetFilename = mapData.tileSetSources[i].GetTileSetFilename() + ".bmp";

		//TODO: Allow FreeImage to take a pointer to the associated well within a vol file 
		//      instead of forcing its extraction.
		bool extracted = resourceManager.ExtractSpecificFile(tileSetFilename);
		
		if (!extracted) {
			throw runtime_error("Unable to find the tileset " + tileSetFilename + " in the directory or in a given archive (.vol).");
		}

		mapImager.AddTileSet(tileSetFilename, ImageFormat::BMP);

		//TODO: If tilesets are Outpost 2 specific, translate the Outpost 2 specific tilesets into 
		//      standard bmp files for use in rendering map.
		//mapImager.AddTileSetRawBits()
	}
}

void MapImager::SetRenderTiles(MapData& mapData, RenderManager& renderManager)
{
	for (unsigned int y = 0; y < mapData.mapHeader.mapTileHeight; y++) {
		for (unsigned int x = 0; x < mapData.mapHeader.mapTileWidth(); x++) {
			renderManager.PasteTile(mapData.GetTileSetIndex(x, y), mapData.GetImageIndex(x, y), x, y);
		}
	}
}

bool MapImager::IsSavedGame(string filename)
{
	return XFile::ExtensionMatches(filename, ".OP2");
}
