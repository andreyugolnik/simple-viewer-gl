#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 1;
	static const long MINOR = 1;
	static const long BUILD = 960;
	static const long REVISION = 5255;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 3604;
	#define RC_FILEVERSION 1,1,960,5255
	#define RC_FILEVERSION_STRING "1, 1, 960, 5255\0"
	static const char FULLVERSION_STRING[] = "1.1.960.5255";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 99;
	

}
#endif //VERSION_H
