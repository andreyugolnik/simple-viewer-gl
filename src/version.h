#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 3;
	static const long BUILD = 30;
	static const long REVISION = 160;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 2070;
	#define RC_FILEVERSION 0,3,30,160
	#define RC_FILEVERSION_STRING "0, 3, 30, 160\0"
	static const char FULLVERSION_STRING[] = "0.3.30.160";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 69;
	

}
#endif //VERSION_H
