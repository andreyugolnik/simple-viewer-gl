#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 1;
	static const long MINOR = 9;
	static const long BUILD = 1663;
	static const long REVISION = 9149;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 4954;
	#define RC_FILEVERSION 1,9,1663,9149
	#define RC_FILEVERSION_STRING "1, 9, 1663, 9149\0"
	static const char FULLVERSION_STRING[] = "1.9.1663.9149";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 2;
	

}
#endif //VERSION_H
