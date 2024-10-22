﻿#if _MSC_VER >= 1600    // VC2010
#pragma execution_character_set("utf-8")
#endif

#ifndef _CONFIG_H_
#define _CONFIG_H_

/*
** Ensure these symbols were not defined by some previous header file.
*/
#ifdef ROBOKIT_VERSION
# undef ROBOKIT_VERSION
#endif
#ifdef ROBOKIT_VERSION_NUMBER
# undef ROBOKIT_VERSION_NUMBER
#endif

#define ROBOKIT_VERSION "1.1.3"
#define ROBOKIT_FULL_VERSION "1.1.3"
#define ROBOKIT_VERSION_NUMBER 1001003
#define ROBOKIT_VERSION_MAJOR 1
#define ROBOKIT_VERSION_MINOR 1
#define ROBOKIT_VERSION_PATCH 3
#define ROBOKIT_PRE_VERSION ""
#define ROBOKIT_SOURCE_ID  // "2017-02-13 16:02:40 ada05cfa86ad7f5645450ac7a2a21c9aa6e57d2c"

#define RBK_MODEL_VERSION "1.0.0"

#ifndef RBK_USE_CPP11
#if defined(__cpp_decltype) || __cplusplus >= 201103L ||                       \
    (defined(_MSC_VER) && _MSC_VER >= 1800)
#define RBK_USE_CPP11 1
#else
#define RBK_USE_CPP11 0
#endif
#endif


#ifndef RBK_EXTERN
# define RBK_EXTERN extern
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
// Windows
#define RBK_SYS_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif

#elif defined(linux) || defined(__linux)
// Linux
#define RBK_SYS_LINUX
#elif defined(__APPLE__) || defined(MACOSX) || defined(macintosh) || defined(Macintosh)
// MacOS
#define RBK_SYS_MACOS
#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
// FreeBSD
#define RBK_SYS_FREEBSD
#else
// Unsupported system
#error This operating system is not supported by this library

#endif

#ifdef RBK_SYS_WINDOWS
#define RBK_LIB_EXTENSION "dll"
#define RBK_PLUGIN_EXPORT __declspec(dllexport)
#pragma warning(disable:4251)
#ifdef RBK_SHARED_LIB
#define RBK_API __declspec(dllexport)
#else
#define RBK_API __declspec(dllimport)
#endif

#if _MSC_VER <= 1600
#define _WEBSOCKETPP_NO_CPP11_SYSTEM_ERROR_
#endif
#elif defined(RBK_SYS_MACOS)
#define RBK_LIB_EXTENSION "dylib"
#define RBK_API __attribute__((visibility("default")))
#define RBK_PLUGIN_EXPORT __attribute__((visibility("default")))
#elif defined(RBK_SYS_LINUX) || defined(RBK_SYS_FREEBSD)
#define RBK_LIB_EXTENSION "so"
#define RBK_API __attribute__((visibility("default")))
#define RBK_PLUGIN_EXPORT __attribute__((visibility("default")))
#else
   // unknown library file type
#error Unknown library file extension for this operating system
#endif

#define RBK_DLL_EXPORT RBK_API

// RBK_API RBK_EXTERN const char robokit_version[];
// RBK_API const char *robokit_sourceid(void);

#ifdef RBK_SYS_WINDOWS
// #pragma deprecated(XXX) XXX is the function name
#define RBK_DEPRECATED __declspec(deprecated)
#elif defined(RBK_SYS_MACOS)
#define RBK_DEPRECATED __attribute__((deprecated))
#elif defined(RBK_SYS_LINUX)
#define RBK_DEPRECATED __attribute__((deprecated))
#elif defined(RBK_SYS_FREEBSD)
#define RBK_DEPRECATED __attribute__((deprecated))
#else
#define RBK_DEPRECATED
#endif

#define RBK_EXPERIMENTAL

#ifdef UNUSED
#elif defined(__GNUC__)
# define UNUSED(x) UNUSED_ ## x __attribute__((unused))
#elif defined(__LCLINT__)
# define UNUSED(x) /*@unused@*/ x
#else
# define UNUSED(x) x
#endif

#define MAX_WEBSOCKET_BUFFERED_AMOUNT 3000000
#define MAX_WEBSOCKET_MESSAGE_SIZE 3000000

#ifdef _MSC_VER
// Windows7 = 0x0601, Windows8 = 0x0602, Windows8.1 = 0x0603, Windows10 = 0x0A00
#define RBK_WINVER 0x601
#define WINVER RBK_WINVER
#define _WIN32_WINNT RBK_WINVER
#define BOOST_USE_WINAPI_VERSION RBK_WINVER
#endif

#define BOOST_LOG_DYN_LINK 1
#define BOOST_LOG_SETUP_DYN_LINK 1
#define BOOST_ALL_DYN_LINK 1

#define BOOST_LIB_DIAGNOSTIC

// for debug boost handler tracking
// #define BOOST_ASIO_ENABLE_HANDLER_TRACKING

#define RBK_TCP_CLIENT_READ_BUFFER_SIZE 102400
#define RBK_SERIAL_PORT_READ_BUFFER_SIZE 1024

//#define RBK_RELEASE

#ifndef RBK_DEFAULT_THREADPOOL_SIZE
#define RBK_DEFAULT_THREADPOOL_SIZE 30
#endif

#ifndef RBK_DEFAULT_THREADPOOL_STEP
#define RBK_DEFAULT_THREADPOOL_STEP 5
#endif

#ifndef RBK_MAX_THREADPOOL_SIZE
#define RBK_MAX_THREADPOOL_SIZE 100
#endif

namespace rbk {
    RBK_API unsigned int getMajorVersion();

    RBK_API unsigned int getMinorVersion();

    RBK_API unsigned int getPatchVersion();
}

#endif // ~_CONFIG_H_
