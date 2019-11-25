# OP2MapImager

Outpost 2 Map and Saved Game Imager utility. This utility allows rendering a .map or .op2 saved game file into an image file (.png, .jpg, .bmp). It supports individual map imaging as well as batch processing of all maps in a folder. Check the [OP2MapImager landing page](https://wiki.outpost2.net/doku.php?id=outpost_2:helper_programs:op2mapimager) for the newest version. Developed by Hooman and Brett208 (Vagabond).

## USAGE NOTES
  * Capable of saving multiple map files and entire directories.
  * The OP2MapImager executable and FreeImage.dll must be in the same directory as the tileset BMPs.
  * Scale Factor (-s) determines the final render size and represents the final pixel length of a single tile
    * Max Value: 32, renders at 32 pixels per tile (full size detail pane view)
    * Min Value: 1, renders at 1 pixel per tile (minimap view)

## EXAMPLE COMMANDS
  * OP2MapImager mapFilename.[map|OP2]
  * OP2MapImager -s 16 -o -q Ashes.map eden01.map sgame0.op2
  * OP2MapImager --Scale 8 --ImageFormat BMP [Directory of choice]

## OPTIONAL ARGUMENTS
  -H / --Help: Displays Help File
  -Q / --Quiet: [Default false] Add switch to run application without issuing console messages.
  -O / --Overwrite: [Default false] Add switch to allow application to overwrite existing files.
  -D / --DestinationDirectory: [Default MapRenders]. Add switch and name of new destination path.
  -I / --ImageFormat: [Default PNG]. Allows PNG|JPG|BMP. Sets the image format of the final render.
  -S / --Scale: [Default 4] Sets Scale Factor of image.
  -A / --AccessArchives: [Default true]. Add switch to disable searching VOL archives for map and well files.

For more information about Outpost 2 visit the Outpost Universe (http://outpost2.net/).
Image Manipulation accomplished through FreeImage (http://freeimage.sourceforge.net/).


----


## INSTALL DIRECTIONS

Ensure OP2MapImager.exe and FreeImage.dll are in the same directory. They can be placed in the root OP2 install directory for ease of install and use. Advanced users may want to establish an environment variable to allow application's use in any directory.

Open a command prompt. Navigate to the directory with OP2MapImager.exe and type 'OP2MapImager -h' to see Usage Message.


## OUTPOST 2 TILESETS (WELL00XX.BMP)

The original tileset images (Wells) shipped with Outpost 2 use a special format that prevents them from being opened by typical pixel image manipulation software (like Gimp or Paint). If you attempt to render a map using the original Outpost 2 tileset images, OP2MapImager will throw an error. The current released version of Outpost 2 contains normally formatted tilesets and will not cause issues with OP2MapImager.

The tilesets (wells) must exist in the same directory as the map(s) chosen to render. The map and the tilesets may remain archived in their volume files for rendering as OP2MapImager is able to access volume files. OP2MapImager will first search for files loosely in the directory and then in the volume files alphabetically if the file does not exist loosely. Archive search may be disabled using the AccessArchives flag.


## SOURCE CODE LOCATION AND COMPILIATION

Source code may be found at: https://github.com/OutpostUniverse/OP2MapImager. See the file DeveloperReadMe.txt in the source code for specific instructions.


## Change Log

Ver 2.1.0
 * Remove Windows specific dependencies from code base (may now be compiled for use on Linux).
 * Allow compiling on both x86 (win32) and x64.
 * Allow accessing tilesets (wells) without removing them from archive files.
 * Bug Fix: Multiple directory search problems preventing OP2MapImager from finding tilesets or maps.
 * Bug Fix: Program crash when unable to find map file.

Ver 2.0.1 (07Nov2017)
  * Fixed security concern when outputting error messages from FreeImage.
  * Fixed bug not allowing rendering of loose maps in current working directory.
  * Reordered change log to be newest version first.
  * Added PDB file zipping to post build event script.

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
