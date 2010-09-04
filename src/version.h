#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 1;
	static const long MINOR = 10;
	static const long BUILD = 1792;
	static const long REVISION = 9847;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 5158;
	#define RC_FILEVERSION 1,10,1792,9847
	#define RC_FILEVERSION_STRING "1, 10, 1792, 9847\0"
	static const char FULLVERSION_STRING[] = "1.10.1792.9847";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 31;
	

}
#endif //VERSION_H
