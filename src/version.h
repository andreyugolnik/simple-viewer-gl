#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 1;
	static const long MINOR = 9;
	static const long BUILD = 1662;
	static const long REVISION = 9147;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 4951;
	#define RC_FILEVERSION 1,9,1662,9147
	#define RC_FILEVERSION_STRING "1, 9, 1662, 9147\0"
	static const char FULLVERSION_STRING[] = "1.9.1662.9147";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 1;
	

}
#endif //VERSION_H
