#ifndef _RBK_FILESYSTEM_H_
#define _RBK_FILESYSTEM_H_

#include <vector>

#include <robokit/config.h>

#include <boost/system/error_code.hpp>

namespace rbk {
    RBK_DLL_EXPORT void listFilesInDirectory(std::vector<std::string>& list,
        const std::string& dir,
        const std::string& extension,
        const bool& fullPath,
        const bool& withExtension,
        boost::system::error_code &ec);

    RBK_DLL_EXPORT std::string constructPath(const std::string& dir, const std::string& filename, const std::string& extenstion = "");

    RBK_DLL_EXPORT std::string getNativePath(const std::string& path);

    RBK_DLL_EXPORT bool pathExists(const std::string& path);

    RBK_DLL_EXPORT bool pathExists(const std::string& path, boost::system::error_code& ec);

    RBK_DLL_EXPORT std::string currentPath();

    RBK_DLL_EXPORT std::string currentPath(boost::system::error_code& ec);

    RBK_DLL_EXPORT bool isFile(const std::string& path);

    RBK_DLL_EXPORT bool isFile(const std::string& path, boost::system::error_code& ec);

    RBK_DLL_EXPORT bool isDirectory(const std::string& path);

    RBK_DLL_EXPORT bool isDirectory(const std::string& path, boost::system::error_code& ec);

    RBK_DLL_EXPORT bool isEmpty(const std::string& path);

    RBK_DLL_EXPORT bool isEmpty(const std::string& path, boost::system::error_code& ec);

    RBK_DLL_EXPORT bool deleteFile(const std::string& path);

    RBK_DLL_EXPORT bool deleteFile(const std::string& path, boost::system::error_code& ec);

    RBK_DLL_EXPORT uintmax_t deleteDirectory(const std::string& path);

    RBK_DLL_EXPORT uintmax_t deleteDirectory(const std::string& path, boost::system::error_code& ec);

    RBK_DLL_EXPORT bool createDirectory(const std::string& path);

    RBK_DLL_EXPORT bool createDirectory(const std::string& path, boost::system::error_code& ec);

    RBK_DLL_EXPORT bool createDirectories(const std::string& path);

    RBK_DLL_EXPORT bool createDirectories(const std::string& path, boost::system::error_code& ec);

    RBK_DLL_EXPORT void copy(const std::string& from, const std::string& to);

    RBK_DLL_EXPORT void copy(const std::string& from, const std::string& to, boost::system::error_code& ec);

    RBK_DLL_EXPORT void copyFile(const std::string& from, const std::string& to, bool overwrite = false);

    RBK_DLL_EXPORT void copyFile(const std::string& from, const std::string& to, bool overwrite, boost::system::error_code& ec);

    RBK_DLL_EXPORT void copyDirectory(const std::string& from, const std::string& to);

    RBK_DLL_EXPORT void copyDirectory(const std::string& from, const std::string& to, boost::system::error_code& ec);

    RBK_DLL_EXPORT uintmax_t fileSize(const std::string& path);

    RBK_DLL_EXPORT uintmax_t fileSize(const std::string& path, boost::system::error_code& ec);
} // namespace rbk

#endif // ~_RBK_FILESYSTEM_H_
