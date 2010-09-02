#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 1;
	static const long MINOR = 8;
	static const long BUILD = 1603;
	static const long REVISION = 8828;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 4826;
	#define RC_FILEVERSION 1,8,1603,8828
	#define RC_FILEVERSION_STRING "1, 8, 1603, 8828\0"
	static const char FULLVERSION_STRING[] = "1.8.1603.8828";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 42;
	

}
#endif //VERSION_H
