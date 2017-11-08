OP2MapImager - Outpost 2 Map and Saved Game Imager
Developed by Hooman and Brett208 (Vagabond)

Check the OP2MapImager landing page for the newest version at https://wiki.outpost2.net/doku.php?id=outpost_2:helper_programs:op2mapimager.

+++ USAGE NOTES +++
  * Capable of saving multiple map files and entire directories.
  * The OP2MapImager executable and FreeImage.dll must be in the same directory as the tileset BMPs.
  * Scale Factor (-s) determines the final render size and represents the final pixel length of a single tile
    * Max Value: 32, renders at full size, or 32 pixels per tile.
    * Min Value: 1, renders at 1 pixel per tile

+++ EXAMPLE COMMANDS +++
  * OP2MapImager mapFilename.[map|OP2]
  * OP2MapImager -s 16 -o -q Ashes.map eden01.map sgame0.op2
  * OP2MapImager --Scale 8 --ImageFormat BMP [Directory of choice]

+++ OPTIONAL ARGUMENTS +++
  -H / --Help: Displays Help File
  -Q / --Quiet: [Default false] Add switch to run application without issuing console messages.
  -O / --Overwrite: [Default false] Add switch to allow application to overwrite existing files.
  -D / --DestinationDirectory: [Default MapRenders]. Add switch and name of new destination path.
  -I / --ImageFormat: [Default PNG]. Allows PNG|JPG|BMP. Sets the image format of the final render.
  -S / --Scale: [Default 4] Sets Scale Factor of image.
  -A / --AccessArchives [Default true]. Add switch to disable searching VOL archives for map and well files.

For more information about Outpost 2 visit the Outpost Universe (http://outpost2.net/).
Image Manipulation accomplished through FreeImage (http://freeimage.sourceforge.net/).


+ + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +


+++ INSTALL DIRECTIONS +++

Ensure OP2MapImager.exe, FreeImage.dll, and well0000.bmp through well0012.bmp are all in the same directory. (I typically put them all in the root OP2 install directory for ease of use, but not required.)

Open a command prompt. Navigate to the directory with OP2MapImager.exe and type 'OP2MapImager -h' to see Usage Message.


+++ OUTPOST 2 TILESETS (WELL00XX.BMP) +++

The original tileset images shipped with Outpost 2 use a special format that prevents them from being opened by typical pixel image manipulation software (like Gimp or Paint). However, Outpost 2 is capable of reading normally formatted 8bpp indexed BMPs.

OP2MapImager contains a copy of the tileset images that have been reformatted from the OP2 specific format to general BMP format. These may be placed in the root directory of Outpost 2 if you wish. They must exist in the same directory as OP2MapImager.exe and FreeImage.dll.

If you attempt to render a map using the original Outpost 2 tileset images, OP2MapImager will throw an error.


+++ ARCHIVE (.VOL) FILE ACCESS +++

By default, OP2MapImager is able to search through .vol archives and pull maps or tilesets out of the .vol files.

Unless the AccessArchives flag is disabled, OP2MapImager will first search the supplied directory, and if it cannot find the file of interest, then it will search alphabetically through all .vol files in the directory for the given file. If the file exists both loosely in the directory and in a .vol file, the copy not in the .vol file will be used.


+++ SOURCE CODE LOCATION AND COMPILIATION +++

Source code may be found at: https://github.com/OutpostUniverse/OP2MapImager. See the file DeveloperReadMe.txt in the source code for specific instructions.


+++ Change Log +++

Ver 2.0.1 (07Nov2017)
  * Fix bug not allowing rendering of loose maps in current working directory.
  * Reorder change log to be newest version first.
  * Add PDB file zipping to post build event script.

Ver 2.0.0 (12Oct2017)
  * Fixed crash if supplying an empty variable ("") for a command via the command prompt.
  * Changed method to provide current directory to use either '.' or './' instead of '/' or '\'.
  * Made the function XFile::replaceFilename platform agnostic.
  * Fixed typos in ReadMe.txt.
  * Removed namespace using statements from header files.
  * Removed Microsoft specific exception classes.
  * Removed Microsoft specific for each loops and replaced with C++ standard range loops.
  * Fixed crashing bug when attempting to render a map outside of the application's current working directory.

Ver 1.0.1 (23Jul2017)
 * Prevented creation of multiple renders of the same map if the map exists in multiple places in the same directory (IE both loosely in directory and in an archive).
 * When imaging all maps & saved games in a directory, prevented selection of wellpallet.map and SGAME10.OP2 since they are not valid maps.
 * Fixed typos in ReadMe.
 * Added Changelog.

Ver 1.0.0 (22Jul2017)
 * Initial Release