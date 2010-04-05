#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 6;
	static const long BUILD = 287;
	static const long REVISION = 1620;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 2487;
	#define RC_FILEVERSION 0,6,287,1620
	#define RC_FILEVERSION_STRING "0, 6, 287, 1620\0"
	static const char FULLVERSION_STRING[] = "0.6.287.1620";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 26;
	

}
#endif //VERSION_H
