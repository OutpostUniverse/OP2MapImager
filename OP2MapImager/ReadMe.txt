OP2MapImager - Outpost 2 Map and Saved Game Imager
Developed by Hooman and Brett208 (Vagabond)

+++ USAGE NOTES +++
  * Capable of saving multiple map files and entire directories.
  * The OP2MapImager executable and FreeImage.DLL must be in the same directory as the tileset BMPs.
  * Tileset BMPs and map files must be manually extracted from VOL files before use with OP2MapImager.
  * Scale Factor determines the final render size and represents the final pixel length of a single tile
    * Max Value: 32, renders at full size, or 32 pixels per tile.
    * Min Value: 1, renders at 1 pixel per tile

+++ EXAMPLE COMMANDS +++
  * OP2MapImager mapFilename.[map|OP2]
  * OP2MapImager -s 16 -o true -q true Ashes.map eden01.map sgame0.op2
  * OP2MapImager --Scale 8 --ImageFormat BMP [Directory of choice]

+++ OPTIONAL ARGUMENTS +++
  -H / --Help: Displays Help File
  -Q / --Quiet: [Defaul false] Makes application run without issueing console messages.
  -O / --Overwrite: [Default false] Allows application to overwrite existing files when rendering new maps.
  -D / --DestinationDirectory:
  -I / --ImageFormat: [Default PNG]. Allows PNG|JPG|BMP. Sets the image format of the final prameter.
  -S / --Scale: [Default 4] Sets Scale Factor of image.

For more information about Outpost 2 visit the Outpost Universe (http://outpost2.net/)
Image Manipulation accomplished through FreeImage (http://freeimage.sourceforge.net/)