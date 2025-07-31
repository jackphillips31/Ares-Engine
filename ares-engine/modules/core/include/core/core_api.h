#ifndef ARES_CORE_API_H
#define ARES_CORE_API_H
#ifdef ARES_STATIC_BUILD
	#define ARES_CORE_API
#else
	#ifdef ARES_CORE_EXPORTS
		#define ARES_CORE_API __declspec(dllexport)
	#else
		#define ARES_CORE_API __declspec(dllimport)
	#endif
#endif
#endif // ARES_CORE_API_H