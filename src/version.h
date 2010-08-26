#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 1;
	static const long MINOR = 5;
	static const long BUILD = 1341;
	static const long REVISION = 7292;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 4360;
	#define RC_FILEVERSION 1,5,1341,7292
	#define RC_FILEVERSION_STRING "1, 5, 1341, 7292\0"
	static const char FULLVERSION_STRING[] = "1.5.1341.7292";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 80;
	

}
#endif //VERSION_H
