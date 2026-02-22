#ifdef _WIN32
  #ifdef DISCOVERY_EXPORTS
    #define DISCOVERY_API __declspec(dllexport)
  #else
    #define DISCOVERY_API __declspec(dllimport)
  #endif
#else
  #define DISCOVERY_API
#endif

DISCOVERY_API int discover_browsers(void);
