/**
 * @addtogroup Declarations
 *
 * Declarations used by the rest of the project.
 *
 * This is intended to be included at the top of all libSRS API headers and sources.
 * It is important that this is included before any standard headers as it may include feature macro definitions.
 *
 * @{
 */
#ifndef _SRS_DECL_H
#define _SRS_DECL_H

/* Good resource: https://sourceforge.net/p/predef/wiki/Home/ */

#ifdef _WIN32
#define kiokuAPI __declspec(dllexport)
#else
#define kiokuAPI
#endif

#ifdef __cplusplus
#define kiokuAPI extern "C" kiokuAPI
#endif

#define KIOKU_DIRSEP "/"

#ifdef __APPLE__
  /* https://stackoverflow.com/questions/6802903/c-ifdef-mac-os-x-question#6802945 */
  #define kiokuOS_APPLE
  #include "TargetConditionals.h"
  /* iOS */
  #ifdef TARGET_OS_IPHONE
    #define kiokuOS_IOS
    #define kiokuOS_MOBILE
  /* iOS Simulator */
  #elif TARGET_IPHONE_SIMULATOR
    #define kiokuOS_IOS
    #define kiokuOS_MOBILE
  #elif TARGET_OS_MAC
    /* Other kinds of Mac OS */
  #else
    /* Unsupported platform */
    #error Unsupported Apple OS
  #endif
#elif defined _WIN32 || defined _WIN64
  #define kiokuOS_WINDOWS
#elif defined __linux__ || __gnu_linux__
  #define kiokuOS_LINUX
#elif defined __unix__ || defined __unix
  #define kiokuOS_UNIX
#elif defined __ANDROID__
  #define kiokuOS_ANDROID
  #define kiokuOS_MOBILE
#else
  #error Unsupported OS
#endif

/* https://sourceforge.net/p/predef/wiki/Architectures/ */
#if /* 64-bit */                                                        \
  /* ARM */                                                             \
  defined __amd64__ || defined __x86_64__ || defined __x86_64 ||        \
  defined _MX64 || defined M_AMD64 ||                                   \
  /* ARM64 */                                                           \
  defined __aarch64__ ||                                                \
  /* Intel Itanium (IA-64) */                                           \
  defined __ia64__  || defined _M_IA64 || defined __itanium__ || 	\
  defined _WIN64
  #define kiokuARCH_64BIT
#elif /* 32-bit */ \
  defined __i386__ || defined _M_IX86 || defined _X86_ || _WIN32
  #define kiokuARCH_32BIT
#else
  #error Unsupported architecture
#endif

/** TODO This is not exactly standard and will need some special casing for some compilers */
/* https://stackoverflow.com/questions/4384765/whats-the-difference-between-pretty-function-function-func */
#if defined __GNUC__
#define srs__FUNCTION_NAME__() __PRETTY_FUNCTION__
#else
#define srs__FUNCTION_NAME__() __func__
#endif

#define srsFUNCTION_NAME srs__FUNCTION_NAME__()

#ifdef kiokuOS_WINDOWS
#define kiokuSTRING_LF "\r\n"
#else
#define kiokuSTRING_LF "\n"
#endif

#define kiokuCHAR_NULL '\0'

/* https://stackoverflow.com/questions/18298280/how-to-declare-a-variable-as-thread-local-portably */
#ifdef __GNUC__
# define srsTHREADLOCAL __thread
#elif __STDC_VERSION__ >= 201112L
# define srsTHREADLOCAL _Thread_local
#elif defined(_MSC_VER)
# define srsTHREADLOCAL __declspec(thread)
#else
# error Cannot define srsTHREADLOCAL
#endif

#endif /* _SRS_DECL_H */

/** @} */
