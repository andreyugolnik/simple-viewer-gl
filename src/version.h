#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 1;
	static const long MINOR = 0;
	static const long BUILD = 853;
	static const long REVISION = 4682;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 3460;
	#define RC_FILEVERSION 1,0,853,4682
	#define RC_FILEVERSION_STRING "1, 0, 853, 4682\0"
	static const char FULLVERSION_STRING[] = "1.0.853.4682";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 92;
	

}
#endif //VERSION_H
