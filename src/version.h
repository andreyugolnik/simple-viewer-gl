#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 2;
	static const long MINOR = 0;
	static const long BUILD = 1929;
	static const long REVISION = 10544;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 5414;
	#define RC_FILEVERSION 2,0,1929,10544
	#define RC_FILEVERSION_STRING "2, 0, 1929, 10544\0"
	static const char FULLVERSION_STRING[] = "2.0.1929.10544";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 68;
	

}
#endif //VERSION_H
