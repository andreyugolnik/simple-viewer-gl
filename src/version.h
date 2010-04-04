#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 3;
	static const long BUILD = 8;
	static const long REVISION = 37;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 2042;
	#define RC_FILEVERSION 0,3,8,37
	#define RC_FILEVERSION_STRING "0, 3, 8, 37\0"
	static const char FULLVERSION_STRING[] = "0.3.8.37";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 47;
	

}
#endif //VERSION_H
