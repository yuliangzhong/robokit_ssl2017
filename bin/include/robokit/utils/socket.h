#ifndef _RBK_UTILS_SOCKET_H_
#define _RBK_UTILS_SOCKET_H_

#include <robokit/config.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <boost/system/error_code.hpp>

namespace rbk {
    namespace utils {
        namespace socket {
            extern RBK_DLL_EXPORT uint32_t networkToHostLong(uint32_t value);

            extern RBK_DLL_EXPORT uint32_t hostToNetworkLong(uint32_t value);

            extern RBK_DLL_EXPORT uint16_t networkToHostShort(uint16_t value);

            extern RBK_DLL_EXPORT uint16_t hostToNetworkShort(uint16_t value);

            extern RBK_DLL_EXPORT void setKeepAliveParam(int socket, bool on, uint32_t idle, uint32_t interval, uint32_t count);

            extern RBK_DLL_EXPORT void localIP(std::vector<std::string>& ips, boost::system::error_code& ec);
        } // namespace socket
    } // namespace utils
} // namespace rbk

#endif // ~_RBK_UTILS_SOCKET_H_
