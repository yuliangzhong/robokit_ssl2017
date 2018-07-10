#ifndef _RBK_PARAM_H_
#define _RBK_PARAM_H_

#include <string>
#include <map>
#include <typeinfo>

#include <robokit/config.h>
#include <robokit/core/mutex.h>
#include <robokit/core/filesystem.h>
#include <robokit/core/json.h>
#include <robokit/core/logger.h>
#include <robokit/core/thread_pool.h>
#include <robokit/core/error.h>
#include <sqlite3pp/sqlite3pp.h>

#include <boost/atomic/atomic.hpp>
#include <boost/thread.hpp>
#include <boost/mpl/or.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/utility/enable_if.hpp>

namespace rbk {
    class BaseParam;

    template<typename T>
    class Param;

    template<typename T>
    class MutableParam;

    class Params;

    class RBK_API ParamsServer {
    public:
        bool init(const std::string& path, const std::string& dbName);

        bool closeDB();

        sqlite3pp::database* DB() const;

        Params* find(const std::string& pluginName);

        bool insert(Params* pluginParams);

        bool insert(const std::string& pluginName, Params* pluginParams);

        bool remove(const std::string& pluginName);

        bool remove(Params* pluginParams);

        bool update(const std::string& pluginName, Params* pluginParams);

        template<typename T>
        bool set(const std::string& pluginName, const std::string& paramKey, const T& value);

        template<typename T>
        bool save(const std::string& pluginName, const std::string& paramKey, const T& value);

        bool save(const std::string& pluginName);

        bool reload(const std::string& pluginName, const std::string& paramKey);

        bool reload(const std::string& pluginName);

        void empty();

        size_t size();

        void getJsonAPI(const std::string& pluginName, const std::string& key, rapidjson::Document& d);

        static ParamsServer* Instance();

    private:
        struct object_creator {
            // This constructor does nothing more than ensure that Instance()
            // is called before main() begins, thus creating the static
            // SingletonClass object before multithreading race issues can come up.
            object_creator() { ParamsServer::Instance(); }
            inline void do_nothing() const { }
        };

        static object_creator create_object;

        ParamsServer();

        ~ParamsServer();

        ParamsServer(const ParamsServer&);

        ParamsServer& operator= (const ParamsServer&);

        std::map<std::string, Params*> _paramsServer;

        rbk::rwMutex _mutex;

        sqlite3pp::database* _db;

        std::string _dbName;

        bool _opened;
    };

    class RBK_API Params {
    public:
        Params();
        Params(const std::string& pluginName);
        ~Params();
        bool init(const std::string& pluginName);

        template<typename T>
        MutableParam<T>* insert(const std::string& key, const T& value);

        template<typename T>
        MutableParam<T>* insert(MutableParam<T>* newParam);

        bool remove(const std::string& key);

        template<typename T>
        bool remove(MutableParam<T>* param);

        BaseParam* find(const std::string& key);

        template<typename T>
        bool findParam(const std::string& key, T& value, const T& defaultValue, const bool isMutable, const std::string& desc = "") {
            if (!_init.load()) { return false; }

            rbk::writeLock wlock(_mutex);

            auto db = rbk::ParamsServer::Instance()->DB();
            std::string qryString("SELECT Value, OriginValue, Mutable FROM " + _pluginName + " WHERE Key = '" + key + "' LIMIT 1");
            sqlite3pp::query qry(*db, qryString.c_str());
            sqlite3pp::query::iterator i = qry.begin();

            bool found = false;

            try {
                if (i != qry.end()) {
                    if (qry.column_count() == 3 && (*i).get<bool>(2) == isMutable) { // if mutability is different, ignore
                        value = (*i).get<T>(0);
                        found = true;
                    }
                }
                else // not found in database, insert one
                {
                    sqlite3pp::command cmd(*db, std::string("INSERT INTO " + _pluginName + "(Key, Type, Value, OriginValue, Mutable, Desc) VALUES (?, ?, ?, ?, ?, ?)").c_str());
                    cmd.binder() << key << typeid(T).name() << defaultValue << defaultValue << isMutable << desc;
                    cmd.execute();
                }
            }
            catch (const sqlite3pp::database_error& e) {
                LogWarn("Find param " << _pluginName << ":" << key << " error: " << e.what());
                rbk::ErrorCodes::Instance()->setNotice(57001);   // 参数相关操作出错
            }
            catch (const std::exception& e) {
                LogWarn("Find param " << _pluginName << ":" << key << " error: " << e.what());
                rbk::ErrorCodes::Instance()->setNotice(57001);   // 参数相关操作出错
            }

            if (found) {
                if (isMutable) {
                    rbk::json::setInDoc(_mutableJsonDoc, key, value);
                    rbk::json::setInDoc(_jsonDoc, key, (*i).get<T>(1)); // set to the origin value for reload
                }
                else {
                    rbk::json::setInDoc(_jsonDoc, key, value);
                }
            }
            else {
                if (isMutable) {
                    rbk::json::setInDoc(_mutableJsonDoc, key, defaultValue);
                    rbk::json::setInDoc(_jsonDoc, key, defaultValue);
                }
                else {
                    rbk::json::setInDoc(_jsonDoc, key, defaultValue);
                }
            }

            return found;
        }

        template<typename T>
        void setMutableJson(const std::string& key, const T& value) {
            if (!_init.load()) { return; }

            rbk::writeLock wlock(_mutex);
            rbk::json::setInDoc(_mutableJsonDoc, key, value);
        }

        void empty();
        size_t size();
        bool isChanged();
        bool reload();
        bool reload(const std::string& key);

        bool save();

        template<typename T>
        bool save(const std::string& key, const T& value, const bool reload = false) {
            if (!_init.load()) { return false; }

            // if reload, don't set it again
            if (reload || this->set(key, value)) {
                auto db = rbk::ParamsServer::Instance()->DB();
                try {
                    std::string cmdString("UPDATE " + _pluginName + " SET Value = ? WHERE Key = ?");
                    sqlite3pp::command cmd(*db, cmdString.c_str());
                    cmd.binder() << value << key;
                    if (cmd.execute() == SQLITE_OK) {
                        return true;
                    }
                }
                catch (const sqlite3pp::database_error& e) {
                    LogWarn("Save param " << _pluginName << ":" << key << " error: " << e.what());
                    rbk::ErrorCodes::Instance()->setNotice(57001);   // 参数相关操作出错
                }
                catch (const std::exception& e) {
                    LogWarn("Save param " << _pluginName << ":" << key << " error: " << e.what());
                    rbk::ErrorCodes::Instance()->setNotice(57001);   // 参数相关操作出错
                }
            }
            return false;
        }

        template<typename T>
        bool get(const std::string& key, T& value);

        template<typename T>
        bool set(const std::string& key, const T& value);

        std::string pluginName() const;

        void getJsonAPI(const std::string& key, rapidjson::Document& d);

    private:
        Params(const Params&);
        Params& operator= (const Params&);
        std::map<std::string, BaseParam*> _params;
        std::string _pluginName;
        rbk::rwMutex _mutex;
        std::string _jsonFilePath;
        std::string _mutableJsonFilePath;
        rapidjson::Document _mutableJsonDoc;
        rapidjson::Document _jsonDoc;
        boost::atomic<bool> _init;
    };

    class RBK_API BaseParam {
    public:
        virtual ~BaseParam();
        virtual size_t type() const;
        virtual bool isChanged() const;
    protected:
        BaseParam(size_t typeHashCode);
        size_t _type;
        boost::atomic<bool> _changed;
    };

    template<typename T>
    class Param : public BaseParam {
    public:
        // SFINAE(匹配失败不是错误)
        Param(typename boost::enable_if<boost::mpl::or_<boost::is_integral<T>, boost::is_floating_point<T>>>::type* dummy = 0) : BaseParam(typeid(T).hash_code()) {
            _init.clear();
        }

        // SFINAE(匹配失败不是错误)
        Param(const std::string& key, const T& value, const std::string& desc = "", typename boost::enable_if<boost::mpl::or_<boost::is_integral<T>, boost::is_floating_point<T>>>::type* dummy = 0) : BaseParam(typeid(T).hash_code()), _key(key), _value(value), _desc(desc) {
            _init.test_and_set();
        }

        ~Param() {}

        void init(const std::string& key, const T& value, const std::string& desc = "") {
            if (_init.test_and_set()) { return; }
            _key = key;
            _value = value;
            _desc = desc;
        }

        inline const std::string key() const { return _key; }

        inline const std::string desc() const { return _desc; }

        inline T get() const {
            return _value;
        }

        friend std::ostream& operator<< (std::ostream& os, const Param<T>& p) {
            os << p.get();
            return os;
        }

        T operator() () const {
            return _value;
        }

        operator T() const {
            return _value;
        }

    private:
        Param<T>(const Param<T>&);

        Param<T>& operator= (const Param<T>&);

        virtual bool isChanged() const { return false; }

        std::string _key;

        T _value;

        std::string _desc;

        boost::atomic_flag _init;
    };

    // specialization for std::string
    template<>
    class Param<std::string> : public BaseParam {
    public:
        Param() : BaseParam(typeid(std::string).hash_code()) {
            _init.clear();
        }

        Param(const std::string& key, const std::string& value, const std::string& desc = "") : BaseParam(typeid(std::string).hash_code()), _key(key), _value(value), _desc(desc) {
            _init.test_and_set();
        }

        ~Param() {}

        void init(const std::string& key, const std::string& value, const std::string& desc = "") {
            if (_init.test_and_set()) { return; }
            _key = key;
            _value = value;
            _desc = desc;
        }

        inline const std::string key() const { return _key; }

        inline const std::string desc() const { return _desc; }

        inline std::string get() const {
            return _value;
        }

        friend std::ostream& operator<< (std::ostream& os, const Param<std::string>& p) {
            os << p.get();
            return os;
        }

        std::string operator() () const {
            return _value;
        }

        operator std::string() const {
            return _value;
        }

        inline const char* c_str() const {
            return _value.c_str();
        }

        inline size_t length() const {
            return _value.length();
        }

        inline size_t size() const {
            return _value.size();
        }

    private:
        Param(const Param<std::string>&);
        Param& operator= (const Param<std::string>&);
        virtual bool isChanged() const { return false; }
        std::string _key;
        std::string _value;
        std::string _desc;
        boost::atomic_flag _init;
    };

    template<typename T>
    class MutableParam : public BaseParam {
    public:
        // SFINAE(匹配失败不是错误)
        MutableParam(typename boost::enable_if<boost::mpl::or_<boost::is_integral<T>, boost::is_floating_point<T>>>::type* dummy = 0) : BaseParam(typeid(T).hash_code()), _params(nullptr) {
            _init.clear();
        }

        // SFINAE(匹配失败不是错误)
        MutableParam(const std::string& key, const T& value, Params* params, const std::string& desc = "", typename boost::enable_if<boost::mpl::or_<boost::is_integral<T>, boost::is_floating_point<T>>>::type* dummy = 0) : BaseParam(typeid(T).hash_code()), _params(nullptr) {
            _init.clear();
            this->init(key, value, params, desc);
        }

        ~MutableParam() {
            if (_params != nullptr) {
                _params->remove(this);
            }
        }

        void init(const std::string& key, const T& value, Params* params, const std::string& desc = "") {
            if (_init.test_and_set()) { return; }
            _key = key;
            _desc = desc;
            _value = value;
            _params = params;
            if (_params == nullptr) { return; }
            if (_params->insert(this) == nullptr) {
                LogWarn("LoadParam: " << _params->pluginName() << " failed to load mutable param \"" << _key << "\", redefinition detected");
                rbk::ErrorCodes::Instance()->setNotice(57001);   // 参数相关操作出错
            }
        }

        inline const std::string key() const { return _key; }

        inline const std::string desc() const { return _desc; }

        inline bool isChanged() const {
            return _changed.load();
        }

        inline T get() {
            T v = _value.load();
            _changed.store(false);
            return v;
        }

        // for param server to generate json api, won't set the _changed to false
        inline T query() {
            T v = _value.load();
            return v;
        }

        void set(const T& value, const bool& fromParams = false) {
            if (!fromParams && _params != nullptr) {
                _params->setMutableJson(_key, value);
            }
            _value.store(value);
            _changed.store(true);
        }

        void save(const T& value) {
            if (_params != nullptr) {
                _params->save(_key, value);
            }
        }

        void save() {
            if (_params != nullptr) {
                return _params->save(_key, _value);
            }
        }

        bool reload() {
            if (_params != nullptr) {
                return _params->reload(_key);
            }
        }

        friend std::ostream& operator<< (std::ostream& os, MutableParam<T>& p) {
            os << p.get();
            return os;
        }

        friend std::istream& operator >> (std::istream& is, MutableParam<T>& p) {
            T value;
            is >> value;
            p.set(value);
            return is;
        }

        T operator() () {
            return this->get();
        }

        void operator() (const T& value) {
            this->set(value);
        }

        MutableParam<T>& operator= (MutableParam<T>& p) {
            if (this == &p) {
                return *this;
            }
            this->set(p);
            return *this;
        }

        MutableParam<T>(MutableParam<T>& p) : BaseParam(typeid(T).hash_code()) {
            this->set(p);
        }

        MutableParam<T>& operator= (const Param<T>& p) {
            this->set(p);
            return *this;
        }

        MutableParam<T>& operator= (const T& value) {
            this->set(value);
            return *this;
        }

        operator T() {
            return this->get();
        }

    private:
        std::string _key;
        std::string _desc;
        boost::atomic<T> _value;
        Params* _params;
        boost::atomic_flag _init;
    };

    // specialization for std::string
    template<>
    class MutableParam<std::string> : public BaseParam {
    public:
        MutableParam() : BaseParam(typeid(std::string).hash_code()), _params(nullptr) {
            _init.clear();
        }

        MutableParam(std::string key, std::string value, Params* params, const std::string desc = "") : BaseParam(typeid(std::string).hash_code()), _params(nullptr) {
            _init.clear();
            this->init(key, value, params, desc);
        }

        ~MutableParam() {
            if (_params != nullptr) {
                _params->remove(this);
            }
        }

        void init(std::string key, std::string value, Params* params, const std::string desc = "") {
            if (_init.test_and_set()) { return; }
            _key = key;
            _desc = desc;
            _value = value;
            _params = params;
            if (_params == nullptr) { return; }
            if (_params->insert(this) == nullptr) {
                LogWarn("LoadParam: " << _params->pluginName() << " failed to load mutable param \"" << _key << "\", redefinition detected");
                rbk::ErrorCodes::Instance()->setNotice(57001);   // 参数相关操作出错
            }
        }

        inline const std::string key() const { return _key; }

        inline const std::string desc() const { return _desc; }

        inline bool isChanged() const {
            return _changed.load();
        }

        std::string get() {
            rbk::readLock rlock(_mutex);
            _changed.store(false);
            return _value;
        }

        // for param server to generate json api, won't set the _changed to false
        std::string query() {
            rbk::readLock rlock(_mutex);
            return _value;
        }

        void set(const std::string& value, const bool& fromParams = false) {
            if (!fromParams && _params != nullptr) {
                _params->setMutableJson(_key, value);
            }
            // 这把锁一定要放下面，否则可能死锁
            rbk::writeLock wlock(_mutex);
            _value = value;
            _changed.store(true);
        }

        void save(const std::string& value) {
            if (_params != nullptr) {
                _params->save(_key, value);
            }
        }

        bool save() {
            if (_params != nullptr) {
                return _params->save(_key, _value);
            }
        }

        bool reload() {
            if (_params != nullptr) {
                return _params->reload(_key);
            }
        }

        friend std::ostream& operator<< (std::ostream& os, MutableParam<std::string>& p) {
            os << p.get();
            return os;
        }

        friend std::istream& operator >> (std::istream& is, MutableParam<std::string>& p) {
            std::string value;
            is >> value;
            p.set(value);
            return is;
        }

        std::string operator() () {
            return this->get();
        }

        void operator() (const std::string& value) {
            this->set(value);
        }

        MutableParam& operator= (MutableParam<std::string>& p) {
            if (this == &p) {
                return *this;
            }
            this->set(p);
            return *this;
        }

        MutableParam(MutableParam<std::string>& p) : BaseParam(typeid(std::string).hash_code()) {
            this->set(p);
        }

        MutableParam& operator= (const Param<std::string>& p) {
            this->set(p);
            return *this;
        }

        MutableParam& operator= (const std::string& value) {
            this->set(value);
            return *this;
        }

        operator std::string() {
            return this->get();
        }

        const char* c_str() {
            // 这里如果直接 return this->get().c_str() 的话， get() 返回的临时变量会被销毁掉
            rbk::readLock rlock(_mutex);
            _changed.store(false);
            return _value.c_str();
        }

        size_t length() {
            return this->get().length();
        }

        size_t size() {
            return this->get().size();
        }

    private:
        std::string _key;
        std::string _desc;
        std::string _value;
        rbk::rwMutex _mutex;
        Params* _params;
        boost::atomic_flag _init;
    };

    typedef std::map<std::string, BaseParam*>::iterator PARAM_ITER;

    template<typename T>
    MutableParam<T>* Params::insert(const std::string& key, const T& value) {
        if (!_init.load()) { return; }
        rbk::writeLock wlock(_mutex);
        MutableParam<T>* newParam = new MutableParam<T>(key, value, this);
        std::pair<PARAM_ITER, bool> ret = _params.insert(std::make_pair(key, newParam));
        if (ret.second) {
            rbk::json::setInDoc(_mutableJsonDoc, key, value);
            return newParam;
        }
        else {
            return nullptr;
        }
    }

    template<typename T>
    MutableParam<T>* Params::insert(MutableParam<T>* newParam) {
        if (!_init.load()) { return nullptr; }
        rbk::writeLock wlock(_mutex);
        std::pair<PARAM_ITER, bool> ret = _params.insert(std::make_pair(newParam->key(), newParam));
        if (ret.second) {
            rbk::json::setInDoc(_mutableJsonDoc, newParam->key(), newParam->get());
            return newParam;
        }
        else {
            return nullptr;
        }
    }

    template<typename T>
    bool Params::remove(MutableParam<T>* param) {
        if (!_init.load()) { return false; }
        rbk::writeLock wlock(_mutex);
        return (_params.erase(param->key()) != 0);
    }

    template<typename T>
    bool Params::get(const std::string& key, T& value) {
        if (!_init.load()) { return false; }
        rbk::readLock rlock(_mutex);
        PARAM_ITER it = _params.find(key);
        if (it != _params.end()) {
            auto p = dynamic_cast<MutableParam<T>*>(it->second);
            if (p != nullptr) {
                value = p->get();
                return true;
            }
        }
        return false;
    }

    template<typename T>
    bool Params::set(const std::string& key, const T& value) {
        if (!_init.load()) { return false; }
        rbk::writeLock wlock(_mutex);
        PARAM_ITER it = _params.find(key);
        if (it != _params.end()) {
            rbk::json::setInDoc(_mutableJsonDoc, key, value);
            auto p = dynamic_cast<MutableParam<T>*>(it->second);
            if (p != nullptr) {
                p->set(value, true);
                return true;
            }
        }
        return false;
    }

    typedef std::map<std::string, Params*>::iterator PARAMSERVER_ITER;

    template<typename T>
    bool ParamsServer::set(const std::string& pluginName, const std::string& paramKey, const T& value) {
        rbk::writeLock wlock(_mutex);
        rbk::Params* pluginParams = this->find(pluginName);
        if (pluginParams != nullptr) {
            rbk::BaseParam* param = pluginParams->find(paramKey);
            if (param != nullptr) {
                return pluginParams->set(paramKey, value);
            }
        }
        return false;
    }

    template<typename T>
    bool ParamsServer::save(const std::string& pluginName, const std::string& paramKey, const T& value) {
        rbk::writeLock wlock(_mutex);
        rbk::Params* pluginParams = this->find(pluginName);
        if (pluginParams != nullptr) {
            rbk::BaseParam* param = pluginParams->find(paramKey);
            if (param != nullptr) {
                pluginParams->save(paramKey, value);
                return true;
            }
        }
        return false;
    }
} // namespace rbk

#endif // ~_RBK_PARAM_H_
