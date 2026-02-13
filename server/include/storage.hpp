#pragma once

#include <list>
#include <optional>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

class Storage {
  public:
    Storage(size_t db_count = 16);

    size_t dbsize(size_t db);
    bool   flushdb(size_t db);
    bool   flushall();

    bool                     exists(size_t db);                         // to check if db index is valid
    bool                     exists(size_t db, const std::string& key); // to check if key exists in db
    bool                     del(size_t db, const std::string& key);
    std::string              type(size_t db, const std::string& key);
    std::vector<std::string> keys(size_t db, const std::string& pattern);

    bool expire(size_t db, const std::string& key, int seconds);
    int  ttl(size_t db, const std::string& key);

    bool                       set(size_t db, const std::string& key, const std::string& value);
    std::optional<std::string> get(size_t db, const std::string& key);
    int                        incr(size_t db, const std::string& key);
    int                        decr(size_t db, const std::string& key);
    bool                       append(size_t db, const std::string& key, const std::string& value);

    bool                     lpush(size_t db, const std::string& key, const std::string& value);
    bool                     rpush(size_t db, const std::string& key, const std::string& value);
    bool                     lpop(size_t db, const std::string& key, std::string& value);
    bool                     rpop(size_t db, const std::string& key, std::string& value);
    std::vector<std::string> lrange(size_t db, const std::string& key, int start, int stop);
    int                      llen(size_t db, const std::string& key);

    bool                            sadd(size_t db, const std::string& key, const std::string& member);
    bool                            srem(size_t db, const std::string& key, const std::string& member);
    std::unordered_set<std::string> smembers(size_t db, const std::string& key);
    bool                            sismember(size_t db, const std::string& key, const std::string& member);
    int                             scard(size_t db, const std::string& key);
    std::unordered_set<std::string> sunion(size_t db, const std::vector<std::string>& keys);
    std::unordered_set<std::string> sinter(size_t db, const std::vector<std::string>& keys);

    bool rename(size_t db, const std::string& old_key, const std::string& new_key);

  private:
    enum class ValueType : std::uint8_t {
        STRING,
        LIST,
        SET
    };

    struct Value {
        ValueType                                                                          type;
        std::variant<std::string, std::list<std::string>, std::unordered_set<std::string>> data;
        time_t                                                                             expire_time = 0;
    };

    using Database = std::unordered_map<std::string, Value>;

    std::vector<Database>     m_databases;
    size_t                    m_current_db = 0;
    mutable std::shared_mutex m_mutex;

    bool isExpired(const Value& val) const;
};
