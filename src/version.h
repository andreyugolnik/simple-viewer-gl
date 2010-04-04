#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 3;
	static const long BUILD = 33;
	static const long REVISION = 178;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 2074;
	#define RC_FILEVERSION 0,3,33,178
	#define RC_FILEVERSION_STRING "0, 3, 33, 178\0"
	static const char FULLVERSION_STRING[] = "0.3.33.178";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 72;
	

}
#endif //VERSION_H
