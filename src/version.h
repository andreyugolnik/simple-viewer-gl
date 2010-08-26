#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 1;
	static const long MINOR = 5;
	static const long BUILD = 1335;
	static const long REVISION = 7251;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 4350;
	#define RC_FILEVERSION 1,5,1335,7251
	#define RC_FILEVERSION_STRING "1, 5, 1335, 7251\0"
	static const char FULLVERSION_STRING[] = "1.5.1335.7251";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 74;
	

}
#endif //VERSION_H
