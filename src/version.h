#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 1;
	static const long MINOR = 10;
	static const long BUILD = 1822;
	static const long REVISION = 10004;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 5213;
	#define RC_FILEVERSION 1,10,1822,10004
	#define RC_FILEVERSION_STRING "1, 10, 1822, 10004\0"
	static const char FULLVERSION_STRING[] = "1.10.1822.10004";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 61;
	

}
#endif //VERSION_H
