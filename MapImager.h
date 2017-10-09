#pragma once

#include "OP2Utility/OP2Utility.h"
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
	MapImager() : resourceManager(".") {};
	bool imageMap(std::string& renderFilenameOut, const std::string& filename, const RenderSettings& renderSettings);
	std::string getImageFormatExtension(ImageFormat imageFormat);

private:
	ResourceManager resourceManager;

	void setRenderTiles(MapData& mapData, RenderManager& mapImager);
	void loadTileSets(MapData& mapData, RenderManager& mapImager, bool accessArchives);
	void formatRenderFilename(std::string& renderFilenameOut, const std::string& filename, const RenderSettings& renderSettings);
	std::string createUniqueFilename(const std::string& filename);
	bool MapImager::isSavedGame(std::string filename);
};