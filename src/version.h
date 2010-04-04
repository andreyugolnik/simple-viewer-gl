#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 3;
	static const long BUILD = 31;
	static const long REVISION = 164;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 2071;
	#define RC_FILEVERSION 0,3,31,164
	#define RC_FILEVERSION_STRING "0, 3, 31, 164\0"
	static const char FULLVERSION_STRING[] = "0.3.31.164";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 70;
	

}
#endif //VERSION_H
