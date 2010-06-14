#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 1;
	static const long MINOR = 4;
	static const long BUILD = 1218;
	static const long REVISION = 6655;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 4121;
	#define RC_FILEVERSION 1,4,1218,6655
	#define RC_FILEVERSION_STRING "1, 4, 1218, 6655\0"
	static const char FULLVERSION_STRING[] = "1.4.1218.6655";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 57;
	

}
#endif //VERSION_H
