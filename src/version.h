#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 1;
	static const long MINOR = 1;
	static const long BUILD = 944;
	static const long REVISION = 5173;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 3573;
	#define RC_FILEVERSION 1,1,944,5173
	#define RC_FILEVERSION_STRING "1, 1, 944, 5173\0"
	static const char FULLVERSION_STRING[] = "1.1.944.5173";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 83;
	

}
#endif //VERSION_H
