﻿#ifndef _RBK_SLEEP_H_
#define _RBK_SLEEP_H_

#include <robokit/config.h>

#include <robokit/core/chrono.h>
#include <boost/thread/thread.hpp>

namespace rbk {
    // 毫秒级
    extern RBK_DLL_EXPORT void sleep(uint64_t milli);

    // 忙等待，微秒级
    extern RBK_DLL_EXPORT void wait(uint64_t micro);
} // namespace rbk

#ifdef RBK_SYS_WINDOWS
#include <windows.h>
#define SLEEP(time) Sleep(time)
#elif defined(RBK_SYS_MACOS)
#include <unistd.h>
#define SLEEP(time) usleep(time*1000)
#elif defined(RBK_SYS_LINUX) || defined(RBK_SYS_FREEBSD)
#include <unistd.h>
#define SLEEP(time) usleep(time*1000)
#else
// unknown library file type
#error Unknown library file extension for this operating system
#endif

#endif // ~_RBK_SLEEP_H_
