#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 1;
	static const long MINOR = 10;
	static const long BUILD = 1828;
	static const long REVISION = 10041;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 5223;
	#define RC_FILEVERSION 1,10,1828,10041
	#define RC_FILEVERSION_STRING "1, 10, 1828, 10041\0"
	static const char FULLVERSION_STRING[] = "1.10.1828.10041";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 67;
	

}
#endif //VERSION_H
