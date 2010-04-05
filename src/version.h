#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 4;
	static const long BUILD = 150;
	static const long REVISION = 839;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 2254;
	#define RC_FILEVERSION 0,4,150,839
	#define RC_FILEVERSION_STRING "0, 4, 150, 839\0"
	static const char FULLVERSION_STRING[] = "0.4.150.839";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 89;
	

}
#endif //VERSION_H
