#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 5;
	static const long BUILD = 198;
	static const long REVISION = 1129;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 2334;
	#define RC_FILEVERSION 0,5,198,1129
	#define RC_FILEVERSION_STRING "0, 5, 198, 1129\0"
	static const char FULLVERSION_STRING[] = "0.5.198.1129";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 37;
	

}
#endif //VERSION_H
