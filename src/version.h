#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 10;
	static const long BUILD = 714;
	static const long REVISION = 3905;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 3154;
	#define RC_FILEVERSION 0,10,714,3905
	#define RC_FILEVERSION_STRING "0, 10, 714, 3905\0"
	static const char FULLVERSION_STRING[] = "0.10.714.3905";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 53;
	

}
#endif //VERSION_H
