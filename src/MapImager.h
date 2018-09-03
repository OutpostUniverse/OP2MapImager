#pragma once

#include "OP2Utility.h"
#include "RenderManager.h"
#include <string>

struct RenderSettings
{
	ImageFormat imageFormat = ImageFormat::PNG;
	int scaleFactor = 4;
	std::string destDirectory = "MapRenders";
	bool overwrite = false;
	bool quiet = false;
	bool helpRequested = false;
	bool accessArchives = true;
};

class MapImager
{
public:
	MapImager(std::string directory) : resourceManager(directory) {};
	void ImageMap(std::string& renderFilenameOut, const std::string& filename, const RenderSettings& renderSettings);
	std::string GetImageFormatExtension(ImageFormat imageFormat);

private:
	ResourceManager resourceManager;

	void SetRenderTiles(MapData& mapData, RenderManager& mapImager);
	void LoadTilesets(MapData& mapData, RenderManager& mapImager, bool accessArchives);
	std::string FormatRenderFilename(const std::string& filename, const RenderSettings& renderSettings);
	std::string CreateUniqueFilename(const std::string& filename);
	MapData ReadMap(const std::string& filename, bool accessArchives);
};
