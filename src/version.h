#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 6;
	static const long BUILD = 355;
	static const long REVISION = 1989;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 2593;
	#define RC_FILEVERSION 0,6,355,1989
	#define RC_FILEVERSION_STRING "0, 6, 355, 1989\0"
	static const char FULLVERSION_STRING[] = "0.6.355.1989";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 94;
	

}
#endif //VERSION_H
