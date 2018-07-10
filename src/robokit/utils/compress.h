#ifndef _RBK_UTILS_COMPRESS_H_
#define _RBK_UTILS_COMPRESS_H_

#include <robokit/config.h>

#include <string>

namespace rbk {
    namespace utils {
        RBK_DLL_EXPORT std::string& compress(std::string &input, int level = 6);

        RBK_DLL_EXPORT std::string& decompress(std::string &input);
    } // namespace utils
} // namespace rbk

#endif
