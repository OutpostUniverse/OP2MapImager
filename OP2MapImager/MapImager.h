#pragma once

#include "OP2Utility.h"
#include "RenderManager.h"
#include <string>

using namespace std;

struct RenderSettings
{
	ImageFormat imageFormat = ImageFormat::PNG;
	int scaleFactor = 4;
	string destDirectory = "MapRenders";
	bool overwrite = false;
	bool quiet = false;
	bool helpRequested = false;
	bool accessArchives = true;
};

class MapImager
{
public:
	bool imageMap(string& renderFilenameOut, const string& filename, const RenderSettings& renderSettings);
	string getImageFormatExtension(ImageFormat imageFormat);

private:
	void setRenderTiles(MapData& mapData, RenderManager& mapImager);
	void loadTileSets(MapData& mapData, RenderManager& mapImager, bool accessArchives);
	void formatRenderFilename(string& renderFilenameOut, const string& filename, const RenderSettings& renderSettings);
	string createUniqueFilename(const string& filename);
};