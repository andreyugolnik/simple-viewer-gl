#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 4;
	static const long BUILD = 122;
	static const long REVISION = 690;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 2201;
	#define RC_FILEVERSION 0,4,122,690
	#define RC_FILEVERSION_STRING "0, 4, 122, 690\0"
	static const char FULLVERSION_STRING[] = "0.4.122.690";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 61;
	

}
#endif //VERSION_H
