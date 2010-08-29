#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 1;
	static const long MINOR = 7;
	static const long BUILD = 1553;
	static const long REVISION = 8560;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 4736;
	#define RC_FILEVERSION 1,7,1553,8560
	#define RC_FILEVERSION_STRING "1, 7, 1553, 8560\0"
	static const char FULLVERSION_STRING[] = "1.7.1553.8560";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 92;
	

}
#endif //VERSION_H
