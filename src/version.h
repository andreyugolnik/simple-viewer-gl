#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 1;
	static const long MINOR = 1;
	static const long BUILD = 955;
	static const long REVISION = 5225;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 3596;
	#define RC_FILEVERSION 1,1,955,5225
	#define RC_FILEVERSION_STRING "1, 1, 955, 5225\0"
	static const char FULLVERSION_STRING[] = "1.1.955.5225";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 94;
	

}
#endif //VERSION_H
