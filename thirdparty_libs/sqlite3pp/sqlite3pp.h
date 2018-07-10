// sqlite3pp.h
//
// The MIT License
//
// Copyright (c) 2015 Wongoo Lee (iwongu at gmail dot com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef SQLITE3PP_H
#define SQLITE3PP_H

#define SQLITE3PP_VERSION "1.0.6"
#define SQLITE3PP_VERSION_MAJOR 1
#define SQLITE3PP_VERSION_MINOR 0
#define SQLITE3PP_VERSION_PATCH 6

#include <functional>
#include <iterator>
#include <sqlite3.h>
#include <stdexcept>
#include <string>
#include <tuple>
#include <boost/noncopyable.hpp>
#include <cstdint>

namespace sqlite3pp
{
  namespace ext
  {
    class function;
    class aggregate;
  }

  template <class T>
  struct convert {
#if defined(WIN32) && _MSC_VER < 1900
    typedef int to_int;
#else
	using to_int = int;
#endif
  };

  class null_type {};

  class database : boost::noncopyable
  {
    friend class statement;
    friend class database_error;
    friend class ext::function;
    friend class ext::aggregate;

   public:
#if defined(WIN32) && _MSC_VER < 1900
    typedef std::function<int (int)> busy_handler;
    typedef std::function<int ()> commit_handler;
    typedef std::function<void ()> rollback_handler;
    typedef std::function<void (int, char const*, char const*, long long int)> update_handler;
    typedef std::function<int (int, char const*, char const*, char const*, char const*)> authorize_handler;
    typedef std::function<void (int, int, int)> backup_handler;
#else
    using busy_handler = std::function<int (int)>;
    using commit_handler = std::function<int ()>;
    using rollback_handler = std::function<void ()>;
    using update_handler = std::function<void (int, char const*, char const*, long long int)>;
    using authorize_handler = std::function<int (int, char const*, char const*, char const*, char const*)>;
    using backup_handler = std::function<void (int, int, int)>;
#endif

    explicit database(char const* dbname = nullptr, int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, const char* vfs = nullptr);

    database(database&& db);
    database& operator=(database&& db);

    ~database();

    int connect(char const* dbname, int flags, const char* vfs = nullptr);
    int disconnect();

    int attach(char const* dbname, char const* name);
    int detach(char const* name);

#if defined(WIN32) && _MSC_VER < 1900
    int backup(database& destdb, backup_handler h = backup_handler());
#else
    int backup(database& destdb, backup_handler h = {});
#endif
    int backup(char const* dbname, database& destdb, char const* destdbname, backup_handler h, int step_page = 5);

    long long int last_insert_rowid() const;

    int enable_foreign_keys(bool enable = true);
    int enable_triggers(bool enable = true);
    int enable_extended_result_codes(bool enable = true);

    int changes() const;

    int error_code() const;
    int extended_error_code() const;
    char const* error_msg() const;

    int execute(char const* sql);
    int executef(char const* sql, ...);

    int set_busy_timeout(int ms);

    void set_busy_handler(busy_handler h);
    void set_commit_handler(commit_handler h);
    void set_rollback_handler(rollback_handler h);
    void set_update_handler(update_handler h);
    void set_authorize_handler(authorize_handler h);

   private:
    sqlite3* db_;

    busy_handler bh_;
    commit_handler ch_;
    rollback_handler rh_;
    update_handler uh_;
    authorize_handler ah_;
  };

  class database_error : public std::runtime_error
  {
   public:
    explicit database_error(char const* msg);
    explicit database_error(database& db);
  };

  enum copy_semantic { copy, nocopy };

  class statement :  boost::noncopyable
  {
   public:
    int prepare(char const* stmt);
    int finish();

    int bind(int idx, bool value);
    int bind(int idx, int32_t value);
    int bind(int idx, uint32_t value);
    int bind(int idx, double value);
    int bind(int idx, int64_t value);
    int bind(int idx, uint64_t value);
    int bind(int idx, char const* value, copy_semantic fcopy);
    int bind(int idx, void const* value, int n, copy_semantic fcopy);
    int bind(int idx, std::string const& value, copy_semantic fcopy);
    int bind(int idx);
    int bind(int idx, null_type);

    int bind(char const* name, bool value);
    int bind(char const* name, int32_t value);
    int bind(char const* name, uint32_t value);
    int bind(char const* name, double value);
    int bind(char const* name, int64_t value);
    int bind(char const* name, uint64_t value);
    int bind(char const* name, char const* value, copy_semantic fcopy);
    int bind(char const* name, void const* value, int n, copy_semantic fcopy);
    int bind(char const* name, std::string const& value, copy_semantic fcopy);
    int bind(char const* name);
    int bind(char const* name, null_type);

    int step();
    int reset();

   protected:
    explicit statement(database& db, char const* stmt = nullptr);
    ~statement();

    int prepare_impl(char const* stmt);
    int finish_impl(sqlite3_stmt* stmt);

   protected:
    database& db_;
    sqlite3_stmt* stmt_;
    char const* tail_;
  };

  class command : public statement
  {
   public:
    class bindstream
    {
     public:
      bindstream(command& cmd, int idx);

      template <class T>
      bindstream& operator << (T value) {
        auto rc = cmd_.bind(idx_, value);
        if (rc != SQLITE_OK) {
          throw database_error(cmd_.db_);
        }
        ++idx_;
        return *this;
      }
      bindstream& operator << (char const* value) {
        auto rc = cmd_.bind(idx_, value, copy);
        if (rc != SQLITE_OK) {
          throw database_error(cmd_.db_);
        }
        ++idx_;
        return *this;
      }
      bindstream& operator << (std::string const& value) {
        auto rc = cmd_.bind(idx_, value, copy);
        if (rc != SQLITE_OK) {
          throw database_error(cmd_.db_);
        }
        ++idx_;
        return *this;
      }

     private:
      command& cmd_;
      int idx_;
    };

    explicit command(database& db, char const* stmt = nullptr);

    bindstream binder(int idx = 1);

    int execute();
    int execute_all();
  };

  class query : public statement
  {
   public:
    class rows
    {
     public:
      class getstream
      {
       public:
        getstream(rows* rws, int idx);

        template <class T>
        getstream& operator >> (T& value) {
          value = rws_->get(idx_, T());
          ++idx_;
          return *this;
        }

       private:
        rows* rws_;
        int idx_;
      };

      explicit rows(sqlite3_stmt* stmt);

      int data_count() const;
      int column_type(int idx) const;

      int column_bytes(int idx) const;

      template <class T> T get(int idx) const {
        return get(idx, T());
      }

#if defined(WIN32) && _MSC_VER < 1900
      template <class T1>
      std::tuple<T1> get_columns(typename convert<T1>::to_int idx1) const {
          return std::make_tuple(get(idx1, T1()));
      }

      template <class T1, class T2>
      std::tuple<T1, T2> get_columns(typename convert<T1>::to_int idx1, typename convert<T2>::to_int idx2) const {
          return std::make_tuple(get(idx1, T1()), get(idx2, T2()));
      }

      template <class T1, class T2, class T3>
      std::tuple<T1, T2, T3> get_columns(typename convert<T1>::to_int idx1, typename convert<T2>::to_int idx2, typename convert<T3>::to_int idx3) const {
          return std::make_tuple(get(idx1, T1()), get(idx2, T2()), get(idx3, T3()));
      }

      template <class T1, class T2, class T3, class T4>
      std::tuple<T1, T2, T3, T4> get_columns(typename convert<T1>::to_int idx1, typename convert<T2>::to_int idx2, typename convert<T3>::to_int idx3,
                                             typename convert<T4>::to_int idx4) const {
          return std::make_tuple(get(idx1, T1()), get(idx2, T2()), get(idx3, T3()), get(idx4, T4()));
      }

      template <class T1, class T2, class T3, class T4, class T5>
      std::tuple<T1, T2, T3, T4, T5> get_columns(typename convert<T1>::to_int idx1, typename convert<T2>::to_int idx2, typename convert<T3>::to_int idx3,
                                                 typename convert<T4>::to_int idx4, typename convert<T5>::to_int idx5) const {
              return std::make_tuple(get(idx1, T1()), get(idx2, T2()), get(idx3, T3()), get(idx4, T4()), get(idx5, T5()));
      }

      template <class T1, class T2, class T3, class T4, class T5, class T6>
      std::tuple<T1, T2, T3, T4, T5, T6> get_columns(typename convert<T1>::to_int idx1, typename convert<T2>::to_int idx2, typename convert<T3>::to_int idx3,
                                                     typename convert<T4>::to_int idx4, typename convert<T5>::to_int idx5, typename convert<T6>::to_int idx6) const {
              return std::make_tuple(get(idx1, T1()), get(idx2, T2()), get(idx3, T3()), get(idx4, T4()), get(idx5, T5()), get(idx6, T6()));
      }

      template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
      std::tuple<T1, T2, T3, T4, T5, T6, T7> get_columns(typename convert<T1>::to_int idx1, typename convert<T2>::to_int idx2, typename convert<T3>::to_int idx3,
                                                         typename convert<T4>::to_int idx4, typename convert<T5>::to_int idx5, typename convert<T6>::to_int idx6,
                                                         typename convert<T7>::to_int idx7) const {
              return std::make_tuple(get(idx1, T1()), get(idx2, T2()), get(idx3, T3()), get(idx4, T4()), get(idx5, T5()), get(idx6, T6()), get(idx7, T7()));
      }

      template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
      std::tuple<T1, T2, T3, T4, T5, T6, T7, T8> get_columns(typename convert<T1>::to_int idx1, typename convert<T2>::to_int idx2, typename convert<T3>::to_int idx3,
                                                             typename convert<T4>::to_int idx4, typename convert<T5>::to_int idx5, typename convert<T6>::to_int idx6,
                                                             typename convert<T7>::to_int idx7, typename convert<T8>::to_int idx8) const {
              return std::make_tuple(get(idx1, T1()), get(idx2, T2()), get(idx3, T3()), get(idx4, T4()), get(idx5, T5()), get(idx6, T6()), get(idx7, T7()), get(idx7, T7()));
      }
#else
      template <class... Ts>
      std::tuple<Ts...> get_columns(typename convert<Ts>::to_int... idxs) const {
        return std::make_tuple(get(idxs, Ts())...);
      }
#endif

      getstream getter(int idx = 0);

     private:
      int32_t get(int idx, int32_t) const;
      uint32_t get(int idx, uint32_t) const;
      bool get(int idx, bool) const;
      double get(int idx, double) const;
      int64_t get(int idx, int64_t) const;
      uint64_t get(int idx, uint64_t) const;
      char const* get(int idx, char const*) const;
      std::string get(int idx, std::string) const;
      void const* get(int idx, void const*) const;
      null_type get(int idx, null_type) const;

     private:
      sqlite3_stmt* stmt_;
    };

    class query_iterator
      : public std::iterator<std::input_iterator_tag, rows>
    {
     public:
      query_iterator();
      explicit query_iterator(query* cmd);

      bool operator==(query_iterator const&) const;
      bool operator!=(query_iterator const&) const;

      query_iterator& operator++();

      value_type operator*() const;

     private:
      query* cmd_;
      int rc_;
    };

    explicit query(database& db, char const* stmt = nullptr);

    int column_count() const;

    char const* column_name(int idx) const;
    char const* column_decltype(int idx) const;

#if defined(WIN32) && _MSC_VER < 1900
    typedef query_iterator iterator;
#else
	using iterator = query_iterator;
#endif

    iterator begin();
    iterator end();
  };

  class transaction : boost::noncopyable 
  {
   public:
    explicit transaction(database& db, bool fcommit = false, bool freserve = false);
    ~transaction();

    int commit();
    int rollback();

   private:
    database* db_;
    bool fcommit_;
  };

} // namespace sqlite3pp

#include "sqlite3pp.ipp"

#endif
