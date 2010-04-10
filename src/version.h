#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 1;
	static const long MINOR = 0;
	static const long BUILD = 850;
	static const long REVISION = 4667;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 3456;
	#define RC_FILEVERSION 1,0,850,4667
	#define RC_FILEVERSION_STRING "1, 0, 850, 4667\0"
	static const char FULLVERSION_STRING[] = "1.0.850.4667";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 89;
	

}
#endif //VERSION_H
