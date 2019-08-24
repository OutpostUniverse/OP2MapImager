OP2MapImager Developer and Compilation Instructions
Developed by Hooman and Brett208 (Vagabond)

If you wish to edit and compile OP2MapImager's source code, read this document first. See ReadMe.txt for a basic description of the project, change log, and instructions on use once compiled. 

Source code may be found at: https://github.com/OutpostUniverse/OP2MapImager.

OP2MapImager is written in C++ and the solution/project files are built for Visual Studio 2017. C++11 and C++14 features are used. The currently expiremental C++ standard library filesystem is used and is supported by both VS2017 and GCC. 

OP2MapImager depends on the project OP2Utility. The project must be compiled in x86 or x64. 

OP2Utility may be found at: https://github.com/OutpostUniverse/OP2Utility.

OP2MapImager requires FreeImage for image manipulation. FreeImage dlls are already included in the downloaded source code. Make sure you compile against the proper platform version of FreeImage (x86 or x64). One could also directly compile against FreeImage source and remove the dependency on FreeImage.dll.

The tilesets that came with Outpost 2 are specially formatted and cannot be run by standard image manipulation software or libraries. These custom bitmaps have been reformatted into standard bitmaps. The game run without any issues using these standard bitmaps. These bitmaps do not come with the source code, but must be downloaded and placed in the same directory as the source code for the software to properly read a map.

Standard formatted bitmaps can be found here: https://wiki.outpost2.net/doku.php?id=outpost_2:helper_programs:map_imager.

+ + + RELEASE COMPILATION INSTRUCTIONS + + +

Post Build Event Notes: When in release mode, a command prompt script will run once the source code is compiled. This script automates staging all required files for distribution. Included in the source code is 7za.exe, the 7-zip command line tool that facilitates zipping the final release package. See http://www.7-zip.org/ for source files and https://www.dotnetperls.com/7-zip-examples for examples of use.


 1. Update ReadMe.txt with changes and new version number. See http://semver.org/ for versioning guidelines.
 2. Set new version number at top of OP2MapImager main.cpp AND in post build event batch script.
 3. Tagging current version in repository.
 4. Set Solution Configuration to Release.
 5. Set Solution Platform to x86.
 6. Compile Code. 
 7. The following files will automatically be copied into the zipped directory'OP2MapImager X.X.X': 
    * OP2MapImager.exe (From Release Directory) 
	* FreeImage.dll
	* ReadMe.txt
	* FreeImage liscense-gplv3.txt
	* OP2MapImager.pdb (will be in a separate zip file)
 8. Change platform to x64 and repeat steps 6-7.
 9. Post both compilations and associated PDB files on the GitHub release page.
