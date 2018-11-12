#include "MapImager.h"
#include "OP2Utility.h"
#include <iostream>
#include <memory>
#include <stdexcept>
#include <cstddef>
#include <cstdint>

using namespace std;

void MapImager::ImageMap(string& renderFilenameOut, const string& filename, const RenderSettings& renderSettings)
{
	MapData mapData = ReadMap(filename, renderSettings.accessArchives);

	RenderManager::Initialize();

	RenderManager mapImager(
		mapData.header.MapTileWidth(),
		mapData.header.mapTileHeight, 24, renderSettings.scaleFactor);

	LoadTilesets(mapData, mapImager, renderSettings.accessArchives);
	SetRenderTiles(mapData, mapImager);

	XFile::NewDirectory(renderSettings.destDirectory);
	renderFilenameOut = FormatRenderFilename(filename, renderSettings);

	bool saveSuccess = mapImager.SaveMapImage(renderFilenameOut, renderSettings.imageFormat);

	RenderManager::Deinitialize();

	if (!saveSuccess) {
		throw std::runtime_error("Error encountered when attempting to save " + renderFilenameOut);
	}
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

void MapImager::LoadTilesets(MapData& mapData, RenderManager& mapImager, bool accessArchives)
{
	for (std::size_t i = 0; i < mapData.tilesetSources.size(); ++i)
	{
		if (mapData.tilesetSources[i].numTiles == 0) {
			continue;
		}

		string tilesetFilename(mapData.tilesetSources[i].tilesetFilename + ".bmp");

		auto stream = resourceManager.GetResourceStream(tilesetFilename);

		if (stream == nullptr) {
			throw runtime_error("Unable to find the tileset " + tilesetFilename + " in the directory or in a given archive (.vol).");
		}

		auto streamLength = stream->Length();

		if (streamLength > SIZE_MAX) {
			throw std::runtime_error("Tileset " + tilesetFilename + " is too large for OP2MapImager to load into memory");
		}

		std::vector<BYTE> buffer(static_cast<std::size_t>(streamLength));
		stream->Read(buffer);
		mapImager.AddTileset(&buffer[0], buffer.size());
	}
}

void MapImager::SetRenderTiles(MapData& mapData, RenderManager& renderManager)
{
	for (unsigned int y = 0; y < mapData.header.mapTileHeight; ++y) {
		for (unsigned int x = 0; x < mapData.header.MapTileWidth(); ++x) {
			renderManager.PasteTile(mapData.GetTilesetIndex(x, y), mapData.GetImageIndex(x, y), x, y);
		}
	}
}

MapData MapImager::ReadMap(const string& filename, bool accessArchives)
{
	auto mapStream = resourceManager.GetResourceStream(filename, accessArchives);

	if (mapStream == nullptr) {
		throw std::runtime_error("Unable to locate " + filename + " within directory or within an archive (vol or clm) in the directory.");
	}

	if (XFile::ExtensionMatches(filename, ".OP2")) {
		return MapReader::ReadSavedGame(*mapStream);
	}
	
	return MapReader::ReadMap(*mapStream);
}
