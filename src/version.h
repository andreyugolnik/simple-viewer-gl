#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 4;
	static const long BUILD = 136;
	static const long REVISION = 769;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 2229;
	#define RC_FILEVERSION 0,4,136,769
	#define RC_FILEVERSION_STRING "0, 4, 136, 769\0"
	static const char FULLVERSION_STRING[] = "0.4.136.769";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 75;
	

}
#endif //VERSION_H
