#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 1;
	static const long MINOR = 6;
	static const long BUILD = 1363;
	static const long REVISION = 7439;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 4392;
	#define RC_FILEVERSION 1,6,1363,7439
	#define RC_FILEVERSION_STRING "1, 6, 1363, 7439\0"
	static const char FULLVERSION_STRING[] = "1.6.1363.7439";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 2;
	

}
#endif //VERSION_H
