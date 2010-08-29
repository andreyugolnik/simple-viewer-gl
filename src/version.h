#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 1;
	static const long MINOR = 8;
	static const long BUILD = 1599;
	static const long REVISION = 8803;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 4818;
	#define RC_FILEVERSION 1,8,1599,8803
	#define RC_FILEVERSION_STRING "1, 8, 1599, 8803\0"
	static const char FULLVERSION_STRING[] = "1.8.1599.8803";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 38;
	

}
#endif //VERSION_H
