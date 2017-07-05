OP2MapImager Ver 1.0 - Outpost 2 Map and Saved Game Imager
Developed by Hooman and Brett208 (Vagabond)

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

For more information about Outpost 2 visit the Outpost Universe (http://outpost2.net/)
Image Manipulation accomplished through FreeImage (http://freeimage.sourceforge.net/)


+ + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +


+++ INSTALL DIRECTIONS +++

Ensure OP2MapImager.exe, FreeImage.dll, and well0000.bmp through well0012.bmp are all in the same directory. (I typically put them all in the root OP2 install directory for ease of use, but not required.)

Open a command prompt. Navigate to the directory with MapImager.exe and type 'OP2MapImager -h'.


+++ OUTPOST 2 TILESETS (WELL00XX.BMP) +++

The original tileset images shipped with Outpost 2 use a special format that prevents them from being opened by typical pixel image manipulation software (like Gimp or Paint). However, Outpost 2 is capable of reading normally formatted 8bpp indexed BMPs.

OP2MapImager contains a copy of the tileset images that have been reformatted from the OP2 specific format to general BMP format. These may be placed in the root directory of Outpost 2 if you wish. They must exist in the same directory as OP2MapImager.exe and FreeImage.dll.

If you attempt to render a map using the original Outpost 2 tileset images, OP2MapImager will throw an error.


+++ ARCHIVE (.VOL) FILE ACCESS +++

By default, OP2MapImager is able to search through .vol archives and pull maps or tilesets out of the .vol files. OP2MapImager currently leaves the .map file or tileset image file loose in the directory after pulling the file. These loose copies may be deleted manually by the user without affecting the ability of Outpost 2 to run.

OP2MapImager will first search the supplied directory, and if it cannot find the file of interest, then it will search alphabetically through all .vol files in the directory for the given file. So if the file exists both loosely in the directory and in a .vol file, the copy not in the .vol file will be used.


+++ RELEASE COMPILATION INSTRUCTIONS +++

Source code may be found at: https://svn.outpostuniverse.org:8443/!/#outpost2/view/head/GameResources/OP2MapImager.

 1. Set Solution Configuration to Release.
 2. Set Solution Platform as desired. (x86/x64).
 3. Compile Code.
 4. Determine version number for this release.
 5. Grab the following files, and place them in a zipped directory with name format 'OP2MapImager 1.0 x64': 
    * OP2MapImager.exe (Release Directory) 
	* FreeImage.dll (There are different DLLs for x32 and x64 compilations)
	* Well0000.BMP-Well0012.BMP (Ensure they are the reformated versions that a normal image editor may open.)
	* ReadMe.txt (this file)
	* FreeImage liscense-gplv3.txt
 6. Place the following files in a separate zipped directory with name format 'OP2MapImager 1.0 x64 Debug':
    * OP2MapRender.pdb
	* OP2Utility.pdb
 7. Place both zip files on the Outpost Universe Website.