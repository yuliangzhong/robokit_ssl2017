#ifndef _RBK_MUTEX_H_
#define _RBK_MUTEX_H_

#include <robokit/config.h>

#include <boost/thread/mutex.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/lock_types.hpp>
#include <boost/thread/lock_guard.hpp>

namespace rbk {
#if defined(WIN32) && _MSC_VER < 1900
    typedef boost::shared_lock<boost::shared_mutex> readLock;
    typedef boost::unique_lock<boost::shared_mutex> writeLock;
    typedef boost::upgrade_lock<boost::shared_mutex> upgradeLock;
    typedef boost::upgrade_to_unique_lock<boost::shared_mutex> upgradeToUniqueLock;
    typedef boost::unique_lock<boost::mutex> lock;
    typedef boost::lock_guard<boost::mutex> lockGuard;
    typedef boost::shared_mutex rwMutex;
    typedef boost::shared_mutex sharedMutex;
    typedef boost::mutex mutex;
#else
    using readLock = boost::shared_lock<boost::shared_mutex>;
    using writeLock = boost::unique_lock<boost::shared_mutex>;
    using upgradeLock = boost::upgrade_lock<boost::shared_mutex>;
    using upgradeToUniqueLock = boost::upgrade_to_unique_lock<boost::shared_mutex>;
    using lock = boost::unique_lock<boost::mutex>;
    using lockGuard = boost::lock_guard<boost::mutex>;
    using rwMutex = boost::shared_mutex;
    using sharedMutex = boost::shared_mutex;
    using mutex = boost::mutex;
#endif
} // namespace rbk

#endif // ~_RBK_MUTEX_H_
