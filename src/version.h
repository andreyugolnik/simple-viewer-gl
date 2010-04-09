#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 1;
	static const long MINOR = 0;
	static const long BUILD = 842;
	static const long REVISION = 4626;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 3433;
	#define RC_FILEVERSION 1,0,842,4626
	#define RC_FILEVERSION_STRING "1, 0, 842, 4626\0"
	static const char FULLVERSION_STRING[] = "1.0.842.4626";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 81;
	

}
#endif //VERSION_H
