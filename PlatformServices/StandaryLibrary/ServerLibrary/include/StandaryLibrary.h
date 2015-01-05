#if defined(WIN32)
	#if defined(STANDARYLIBRARY_EXPORTS)
		#define StandaryLibrary_API __declspec(dllexport)
	#else
		#define StandaryLibrary_API __declspec(dllimport)
	#endif
#else
    #define StandaryLibrary_API
#endif
