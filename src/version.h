#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 7;
	static const long BUILD = 372;
	static const long REVISION = 2088;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 2610;
	#define RC_FILEVERSION 0,7,372,2088
	#define RC_FILEVERSION_STRING "0, 7, 372, 2088\0"
	static const char FULLVERSION_STRING[] = "0.7.372.2088";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 11;
	

}
#endif //VERSION_H
