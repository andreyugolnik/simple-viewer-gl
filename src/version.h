#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 2;
	static const long MINOR = 0;
	static const long BUILD = 1905;
	static const long REVISION = 10421;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 5364;
	#define RC_FILEVERSION 2,0,1905,10421
	#define RC_FILEVERSION_STRING "2, 0, 1905, 10421\0"
	static const char FULLVERSION_STRING[] = "2.0.1905.10421";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 44;
	

}
#endif //VERSION_H
