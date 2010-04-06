#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 6;
	static const long BUILD = 358;
	static const long REVISION = 2009;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 2595;
	#define RC_FILEVERSION 0,6,358,2009
	#define RC_FILEVERSION_STRING "0, 6, 358, 2009\0"
	static const char FULLVERSION_STRING[] = "0.6.358.2009";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 97;
	

}
#endif //VERSION_H
