﻿#pragma once
#ifndef _RBK_CORE_RUNTIME_DATA_DB_H_
#define _RBK_CORE_RUNTIME_DATA_DB_H_

#include <robokit/config.h>
#include <sqlite3pp/sqlite3pp.h>
#include <functional>
#include <robokit/core/logger.h>
#include <robokit/core/error.h>

#if defined(WIN32) && _MSC_VER < 1900
#include <boost/chrono.hpp>
#include <boost/atomic/atomic_flag.hpp>
#else
#include <chrono>
#include <atomic>
#endif

namespace rbk {
    namespace core {
        class RBK_API RuntimeDataDB {
        public:
            explicit RuntimeDataDB(const std::string& name);

            ~RuntimeDataDB();

            bool isOpen() const;

            bool open(const std::string& name);

            bool close();

            template <typename T>
            bool add(const std::string& key, const T value) {
                if (_opened && key.length() > 0) {
                    try
                    {
                        _addCmd->reset();
                        _addCmd->binder() << key << value;
                        if (_addCmd->execute() == SQLITE_OK) {
                            return true;
                        }
                    }
                    catch (const sqlite3pp::database_error& e) {
                        LogWarn("Add " << key << " to Runtime Data " << _name << " error: " << e.what());
                        rbk::ErrorCodes::Instance()->setNotice(57002);   // 数据库相关操作出错
                    }
                    catch (const std::exception& e)
                    {
                        LogWarn("Add " << key << " to Runtime Data " << _name << " error: " << e.what());
                        rbk::ErrorCodes::Instance()->setNotice(57002);   // 数据库相关操作出错
                    }
                }
                return false;
            }

            bool del(const std::string& key);

            bool transaction();

            bool commit();

            bool asyncCmd(const std::function<void()>& f);

            bool rollback();

            template <typename T>
            bool set(const std::string& key, const T value) {
                if (_opened && key.length() > 0) {
                    try
                    {
                        _setCmd->reset();
                        _setCmd->binder() << value << key;
                        if (_setCmd->execute() == SQLITE_OK) {
                            return true;
                        }
                    }
                    catch (const sqlite3pp::database_error& e) {
                        LogWarn("Set " << key << " in Runtime Data " << _name << " error: " << e.what());
                        rbk::ErrorCodes::Instance()->setNotice(57002);   // 数据库相关操作出错
                    }
                    catch (const std::exception& e)
                    {
                        LogWarn("Set " << key << " in Runtime Data " << _name << " error: " << e.what());
                        rbk::ErrorCodes::Instance()->setNotice(57002);   // 数据库相关操作出错
                    }
                }
                return false;
            }

            template <typename T>
            bool get(const std::string& key, T& value) {
                if (_opened && key.length() > 0) {
                    try
                    {
                        std::string qryString("SELECT Value FROM RuntimeData WHERE Key = '" + key + "' LIMIT 1");
                        sqlite3pp::query qry(*_db, qryString.c_str());
                        sqlite3pp::query::iterator i = qry.begin();
                        if (i != qry.end()) {
                            value = (*i).get<T>(0);
                            return true;
                        }
                    }
                    catch (const sqlite3pp::database_error& e) {
                        LogWarn("Get " << key << " from Runtime Data " << _name << " error: " << e.what());
                        rbk::ErrorCodes::Instance()->setNotice(57002);   // 数据库相关操作出错
                    }
                    catch (const std::exception& e)
                    {
                        LogWarn("Get " << key << " from Runtime Data " << _name << " error: " << e.what());
                        rbk::ErrorCodes::Instance()->setNotice(57002);   // 数据库相关操作出错
                    }
                }
                return false;
            }

            bool exists(const std::string& key);

            int errorCode() const;

            std::string errorMsg() const;

        private:

#if defined(WIN32) && _MSC_VER < 1900
            RuntimeDataDB() { }
            RuntimeDataDB(const RuntimeDataDB&);
            RuntimeDataDB& operator=(const RuntimeDataDB&);
#else
            RuntimeDataDB() = delete;
            RuntimeDataDB(const RuntimeDataDB&) = delete;
            RuntimeDataDB& operator=(const RuntimeDataDB&) = delete;
#endif

            std::string _name;

            sqlite3pp::database* _db;

            sqlite3pp::transaction* _xct;

            sqlite3pp::command* _setCmd;
            sqlite3pp::command* _addCmd;
            sqlite3pp::command* _delCmd;

#if defined(WIN32) && _MSC_VER < 1900
            boost::atomic_flag _asyncCmdFlag;
#else
            std::atomic_flag _asyncCmdFlag;
#endif

            bool _opened;
        };
    } // namespace core
} // namespace rbk

#endif // ~_RBK_CORE_RUNTIME_DATA_H_
