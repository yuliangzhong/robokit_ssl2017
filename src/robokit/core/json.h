#ifndef _RBK_JSON_H_
#define _RBK_JSON_H_

#include <robokit/config.h>
#include <rapidjson/error/en.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <boost/system/error_code.hpp>
#include <vector>

namespace rbk {
    namespace json {
        typedef rapidjson::Value::MemberIterator MemberIter;
        typedef rapidjson::Value::ValueIterator ValueIter;
        typedef rapidjson::Value::ConstMemberIterator ConstMemberIter;
        typedef rapidjson::Value::ConstValueIterator ConstValueIter;
        typedef rapidjson::PrettyWriter<rapidjson::FileWriteStream> PrettyFileWriter;
        typedef rapidjson::PrettyWriter<rapidjson::StringBuffer> PrettyStringWriter;
        typedef rapidjson::Writer<rapidjson::FileWriteStream> FileWriter;
        typedef rapidjson::Writer<rapidjson::StringBuffer> StringWriter;

        extern RBK_DLL_EXPORT rapidjson::GetParseErrorFunc GetParseError;

        RBK_DLL_EXPORT void readFile(const std::string& path, rapidjson::Document& d, boost::system::error_code& ec);

        RBK_DLL_EXPORT void writeFile(const std::string& path, const rapidjson::Document& d, boost::system::error_code& ec, bool pretty = false);
        RBK_DLL_EXPORT void writeFile(const std::string& path, const rapidjson::Value& v, boost::system::error_code& ec, bool pretty = false);

        template <typename T>
        bool findInDoc(const rapidjson::Document& doc, const std::string& key, T& value) { return false; }

        template <typename T>
        void setInDoc(rapidjson::Document& doc, const std::string& key, const T& value) { return; }

        template <typename T>
        void setInDoc(rapidjson::Document& doc, const std::string& key, const T* value) { return; }

        template <>
        RBK_DLL_EXPORT bool findInDoc<bool>(const rapidjson::Document& doc, const std::string& key, bool& value);

        template <>
        RBK_DLL_EXPORT bool findInDoc<std::vector<bool>>(const rapidjson::Document& doc, const std::string& key, std::vector<bool>& value);

        template <>
        RBK_DLL_EXPORT bool findInDoc<int>(const rapidjson::Document& doc, const std::string& key, int& value);

        template <>
        RBK_DLL_EXPORT bool findInDoc<std::vector<int>>(const rapidjson::Document& doc, const std::string& key, std::vector<int>& value);

        template <>
        RBK_DLL_EXPORT bool findInDoc<unsigned>(const rapidjson::Document& doc, const std::string& key, unsigned& value);

        template <>
        RBK_DLL_EXPORT bool findInDoc<std::vector<unsigned>>(const rapidjson::Document& doc, const std::string& key, std::vector<unsigned>& value);

        template <>
        RBK_DLL_EXPORT bool findInDoc<int64_t>(const rapidjson::Document& doc, const std::string& key, int64_t& value);

        template <>
        RBK_DLL_EXPORT bool findInDoc<std::vector<int64_t>>(const rapidjson::Document& doc, const std::string& key, std::vector<int64_t>& value);

        template <>
        RBK_DLL_EXPORT bool findInDoc<uint64_t>(const rapidjson::Document& doc, const std::string& key, uint64_t& value);

        template <>
        RBK_DLL_EXPORT bool findInDoc<std::vector<uint64_t>>(const rapidjson::Document& doc, const std::string& key, std::vector<uint64_t>& value);

        template <>
        RBK_DLL_EXPORT bool findInDoc<float>(const rapidjson::Document& doc, const std::string& key, float& value);

        template <>
        RBK_DLL_EXPORT bool findInDoc<std::vector<float>>(const rapidjson::Document& doc, const std::string& key, std::vector<float>& value);

        template <>
        RBK_DLL_EXPORT bool findInDoc<double>(const rapidjson::Document& doc, const std::string& key, double& value);

        template <>
        RBK_DLL_EXPORT bool findInDoc<std::vector<double>>(const rapidjson::Document& doc, const std::string& key, std::vector<double>& value);

        template <>
        RBK_DLL_EXPORT bool findInDoc<std::string>(const rapidjson::Document& doc, const std::string& key, std::string& value);

        template <>
        RBK_DLL_EXPORT bool findInDoc<std::vector<std::string>>(const rapidjson::Document& doc, const std::string& key, std::vector<std::string>& value);

        template <>
        RBK_DLL_EXPORT void setInDoc<bool>(rapidjson::Document& doc, const std::string& key, const bool& value);

        template <>
        RBK_DLL_EXPORT void setInDoc<std::vector<bool>>(rapidjson::Document& doc, const std::string& key, const std::vector<bool>& value);

        template <>
        RBK_DLL_EXPORT void setInDoc<int>(rapidjson::Document& doc, const std::string& key, const int& value);

        template <>
        RBK_DLL_EXPORT void setInDoc<std::vector<int>>(rapidjson::Document& doc, const std::string& key, const std::vector<int>& value);

        template <>
        RBK_DLL_EXPORT void setInDoc<unsigned>(rapidjson::Document& doc, const std::string& key, const unsigned& value);

        template <>
        RBK_DLL_EXPORT void setInDoc<std::vector<unsigned>>(rapidjson::Document& doc, const std::string& key, const std::vector<unsigned>& value);

        template <>
        RBK_DLL_EXPORT void setInDoc<int64_t>(rapidjson::Document& doc, const std::string& key, const int64_t& value);

        template <>
        RBK_DLL_EXPORT void setInDoc<std::vector<int64_t>>(rapidjson::Document& doc, const std::string& key, const std::vector<int64_t>& value);

        template <>
        RBK_DLL_EXPORT void setInDoc<uint64_t>(rapidjson::Document& doc, const std::string& key, const uint64_t& value);

        template <>
        RBK_DLL_EXPORT void setInDoc<std::vector<uint64_t>>(rapidjson::Document& doc, const std::string& key, const std::vector<uint64_t>& value);

        template <>
        RBK_DLL_EXPORT void setInDoc<float>(rapidjson::Document& doc, const std::string& key, const float& value);

        template <>
        RBK_DLL_EXPORT void setInDoc<std::vector<float>>(rapidjson::Document& doc, const std::string& key, const std::vector<float>& value);

        template <>
        RBK_DLL_EXPORT void setInDoc<double>(rapidjson::Document& doc, const std::string& key, const double& value);

        template <>
        RBK_DLL_EXPORT void setInDoc<std::vector<double>>(rapidjson::Document& doc, const std::string& key, const std::vector<double>& value);

        template <>
        RBK_DLL_EXPORT void setInDoc<std::string>(rapidjson::Document& doc, const std::string& key, const std::string& value);

        template <>
        RBK_DLL_EXPORT void setInDoc<char>(rapidjson::Document& doc, const std::string& key, const char* value);

        template <>
        RBK_DLL_EXPORT void setInDoc<std::vector<std::string>>(rapidjson::Document& doc, const std::string& key, const std::vector<std::string>& value);

        RBK_DLL_EXPORT bool parse(rapidjson::Document& doc, const std::string& buf);

        RBK_DLL_EXPORT bool parse(rapidjson::Document& doc, const char* buf, const size_t size);

        RBK_DLL_EXPORT bool parse(rapidjson::Document& doc, const std::vector<char>& buffer, const size_t size);

        RBK_DLL_EXPORT bool toString(const rapidjson::Document& doc, std::string& buffer, bool pretty = false);

        RBK_DLL_EXPORT bool toString(const rapidjson::Value& value, std::string& buffer, bool pretty = false);

        template <typename T>
        bool findByPointer(const rapidjson::Document& doc, const std::string& pointer, T& value) { return false; }

        template <typename T>
        void setByPointer(rapidjson::Document& doc, const std::string& pointer, const T& value) { return; }

        template <typename T>
        void setByPointer(rapidjson::Document& doc, const std::string& pointer, const T* value) { return; }

        template <>
        RBK_DLL_EXPORT bool findByPointer<bool>(const rapidjson::Document& doc, const std::string& pointer, bool& value);

        template <>
        RBK_DLL_EXPORT bool findByPointer<std::vector<bool>>(const rapidjson::Document& doc, const std::string& pointer, std::vector<bool>& value);

        template <>
        RBK_DLL_EXPORT bool findByPointer<int>(const rapidjson::Document& doc, const std::string& pointer, int& value);

        template <>
        RBK_DLL_EXPORT bool findByPointer<std::vector<int>>(const rapidjson::Document& doc, const std::string& pointer, std::vector<int>& value);

        template <>
        RBK_DLL_EXPORT bool findByPointer<unsigned>(const rapidjson::Document& doc, const std::string& pointer, unsigned& value);

        template <>
        RBK_DLL_EXPORT bool findByPointer<std::vector<unsigned>>(const rapidjson::Document& doc, const std::string& pointer, std::vector<unsigned>& value);

        template <>
        RBK_DLL_EXPORT bool findByPointer<int8_t>(const rapidjson::Document& doc, const std::string& pointer, int8_t& value);

        template <>
        RBK_DLL_EXPORT bool findByPointer<std::vector<int8_t>>(const rapidjson::Document& doc, const std::string& pointer, std::vector<int8_t>& value);

        template <>
        RBK_DLL_EXPORT bool findByPointer<uint8_t>(const rapidjson::Document& doc, const std::string& pointer, uint8_t& value);

        template <>
        RBK_DLL_EXPORT bool findByPointer<std::vector<uint8_t>>(const rapidjson::Document& doc, const std::string& pointer, std::vector<uint8_t>& value);

        template <>
        RBK_DLL_EXPORT bool findByPointer<int16_t>(const rapidjson::Document& doc, const std::string& pointer, int16_t& value);

        template <>
        RBK_DLL_EXPORT bool findByPointer<std::vector<int16_t>>(const rapidjson::Document& doc, const std::string& pointer, std::vector<int16_t>& value);

        template <>
        RBK_DLL_EXPORT bool findByPointer<uint16_t>(const rapidjson::Document& doc, const std::string& pointer, uint16_t& value);

        template <>
        RBK_DLL_EXPORT bool findByPointer<std::vector<uint16_t>>(const rapidjson::Document& doc, const std::string& pointer, std::vector<uint16_t>& value);

        template <>
        RBK_DLL_EXPORT bool findByPointer<int64_t>(const rapidjson::Document& doc, const std::string& pointer, int64_t& value);

        template <>
        RBK_DLL_EXPORT bool findByPointer<std::vector<int64_t>>(const rapidjson::Document& doc, const std::string& pointer, std::vector<int64_t>& value);

        template <>
        RBK_DLL_EXPORT bool findByPointer<uint64_t>(const rapidjson::Document& doc, const std::string& pointer, uint64_t& value);

        template <>
        RBK_DLL_EXPORT bool findByPointer<std::vector<uint64_t>>(const rapidjson::Document& doc, const std::string& pointer, std::vector<uint64_t>& value);

        template <>
        RBK_DLL_EXPORT bool findByPointer<float>(const rapidjson::Document& doc, const std::string& pointer, float& value);

        template <>
        RBK_DLL_EXPORT bool findByPointer<std::vector<float>>(const rapidjson::Document& doc, const std::string& pointer, std::vector<float>& value);

        template <>
        RBK_DLL_EXPORT bool findByPointer<double>(const rapidjson::Document& doc, const std::string& pointer, double& value);

        template <>
        RBK_DLL_EXPORT bool findByPointer<std::vector<double>>(const rapidjson::Document& doc, const std::string& pointer, std::vector<double>& value);

        template <>
        RBK_DLL_EXPORT bool findByPointer<std::string>(const rapidjson::Document& doc, const std::string& pointer, std::string& value);

        template <>
        RBK_DLL_EXPORT bool findByPointer<std::vector<std::string>>(const rapidjson::Document& doc, const std::string& pointer, std::vector<std::string>& value);

        template <>
        RBK_DLL_EXPORT void setByPointer<bool>(rapidjson::Document& doc, const std::string& pointer, const bool& value);

        template <>
        RBK_DLL_EXPORT void setByPointer<std::vector<bool>>(rapidjson::Document& doc, const std::string& pointer, const std::vector<bool>& value);

        template <>
        RBK_DLL_EXPORT void setByPointer<int>(rapidjson::Document& doc, const std::string& pointer, const int& value);

        template <>
        RBK_DLL_EXPORT void setByPointer<std::vector<int>>(rapidjson::Document& doc, const std::string& pointer, const std::vector<int>& value);

        template <>
        RBK_DLL_EXPORT void setByPointer<unsigned>(rapidjson::Document& doc, const std::string& pointer, const unsigned& value);

        template <>
        RBK_DLL_EXPORT void setByPointer<std::vector<unsigned>>(rapidjson::Document& doc, const std::string& pointer, const std::vector<unsigned>& value);

        template <>
        RBK_DLL_EXPORT void setByPointer<int8_t>(rapidjson::Document& doc, const std::string& pointer, const int8_t& value);

        template <>
        RBK_DLL_EXPORT void setByPointer<std::vector<int8_t>>(rapidjson::Document& doc, const std::string& pointer, const std::vector<int8_t>& value);

        template <>
        RBK_DLL_EXPORT void setByPointer<uint8_t>(rapidjson::Document& doc, const std::string& pointer, const uint8_t& value);

        template <>
        RBK_DLL_EXPORT void setByPointer<std::vector<uint8_t>>(rapidjson::Document& doc, const std::string& pointer, const std::vector<uint8_t>& value);

        template <>
        RBK_DLL_EXPORT void setByPointer<int16_t>(rapidjson::Document& doc, const std::string& pointer, const int16_t& value);

        template <>
        RBK_DLL_EXPORT void setByPointer<std::vector<int16_t>>(rapidjson::Document& doc, const std::string& pointer, const std::vector<int16_t>& value);

        template <>
        RBK_DLL_EXPORT void setByPointer<uint16_t>(rapidjson::Document& doc, const std::string& pointer, const uint16_t& value);

        template <>
        RBK_DLL_EXPORT void setByPointer<std::vector<uint16_t>>(rapidjson::Document& doc, const std::string& pointer, const std::vector<uint16_t>& value);

        template <>
        RBK_DLL_EXPORT void setByPointer<int64_t>(rapidjson::Document& doc, const std::string& pointer, const int64_t& value);

        template <>
        RBK_DLL_EXPORT void setByPointer<std::vector<int64_t>>(rapidjson::Document& doc, const std::string& pointer, const std::vector<int64_t>& value);

        template <>
        RBK_DLL_EXPORT void setByPointer<uint64_t>(rapidjson::Document& doc, const std::string& pointer, const uint64_t& value);

        template <>
        RBK_DLL_EXPORT void setByPointer<std::vector<uint64_t>>(rapidjson::Document& doc, const std::string& pointer, const std::vector<uint64_t>& value);

        template <>
        RBK_DLL_EXPORT void setByPointer<float>(rapidjson::Document& doc, const std::string& pointer, const float& value);

        template <>
        RBK_DLL_EXPORT void setByPointer<std::vector<float>>(rapidjson::Document& doc, const std::string& pointer, const std::vector<float>& value);

        template <>
        RBK_DLL_EXPORT void setByPointer<double>(rapidjson::Document& doc, const std::string& pointer, const double& value);

        template <>
        RBK_DLL_EXPORT void setByPointer<std::vector<double>>(rapidjson::Document& doc, const std::string& pointer, const std::vector<double>& value);

        template <>
        RBK_DLL_EXPORT void setByPointer<std::string>(rapidjson::Document& doc, const std::string& pointer, const std::string& value);

        template <>
        RBK_DLL_EXPORT void setByPointer<char>(rapidjson::Document& doc, const std::string& pointer, const char* value);

        template <>
        RBK_DLL_EXPORT void setByPointer<std::vector<std::string>>(rapidjson::Document& doc, const std::string& pointer, const std::vector<std::string>& value);
    } // namespace json
} // namespace rbk

#endif // ~_RBK_JSON_H_
