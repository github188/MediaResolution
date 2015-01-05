#if defined(WIN32)
	#if defined(DISTRIBUTIONLIBRARY_EXPORTS)
		#define DistributionLibrary_API __declspec(dllexport)
	#else
		#define DistributionLibrary_API __declspec(dllimport)
	#endif
#else
    #define DistributionLibrary_API
#endif