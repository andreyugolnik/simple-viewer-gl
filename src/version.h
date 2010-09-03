#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 1;
	static const long MINOR = 9;
	static const long BUILD = 1702;
	static const long REVISION = 9378;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 5030;
	#define RC_FILEVERSION 1,9,1702,9378
	#define RC_FILEVERSION_STRING "1, 9, 1702, 9378\0"
	static const char FULLVERSION_STRING[] = "1.9.1702.9378";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 41;
	

}
#endif //VERSION_H
