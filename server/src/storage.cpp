#include "storage.hpp"
#include <chrono>
#include <thread>
#include <regex>
#include <algorithm>
#include <mutex>

#include "regex.hpp"

Storage::Storage(size_t db_count) : m_databases(db_count) {}

bool Storage::isExpired(const Value &val) const
{
    if (val.expire_time == 0)
        return false;
    return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) > val.expire_time;
}

size_t Storage::dbsize(size_t db)
{
    if (db >= m_databases.size())
        return 0;
    std::shared_lock lock(m_mutex);
    size_t count = 0;
    for (auto &[key, val] : m_databases[db])
    {
        if (!isExpired(val))
            count++;
    }
    return count;
}

bool Storage::flushdb(size_t db)
{
    if (db >= m_databases.size())
        return false;
    std::unique_lock lock(m_mutex);
    m_databases[db].clear();
    return true;
}

bool Storage::flushall()
{
    std::unique_lock lock(m_mutex);
    for (auto &db : m_databases)
        db.clear();
    return true;
}

bool Storage::exists(size_t db)
{
    return db < m_databases.size();
}

bool Storage::exists(size_t db, const std::string &key)
{
    std::unique_lock lock(m_mutex);
    auto it = m_databases[db].find(key);
    if (it == m_databases[db].end())
        return false;
    if (isExpired(it->second))
    {
        m_databases[db].erase(it);
        return false;
    }
    return true;
}

bool Storage::del(size_t db, const std::string &key)
{
    std::unique_lock lock(m_mutex);
    return m_databases[db].erase(key) > 0;
}

std::string Storage::type(size_t db, const std::string &key)
{
    std::shared_lock lock(m_mutex);
    auto it = m_databases[db].find(key);
    if (it == m_databases[db].end() || isExpired(it->second))
        return "none";
    switch (it->second.type)
    {
    case ValueType::STRING:
        return "string";
    case ValueType::LIST:
        return "list";
    case ValueType::SET:
        return "set";
    }
    return "none";
}

std::vector<std::string> Storage::keys(size_t db, const std::string &pattern)
{
    std::shared_lock lock(m_mutex);
    std::vector<std::string> result;
    std::regex regex_pattern(globToRegex(pattern));
    for (auto &[key, val] : m_databases[db])
    {
        if (!isExpired(val) && std::regex_match(key, regex_pattern))
        {
            result.push_back(key);
        }
    }
    return result;
}

bool Storage::expire(size_t db, const std::string &key, int seconds)
{
    std::unique_lock lock(m_mutex);
    auto it = m_databases[db].find(key);
    if (it == m_databases[db].end())
        return false;
    it->second.expire_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) + seconds;
    return true;
}

int Storage::ttl(size_t db, const std::string &key)
{
    std::shared_lock lock(m_mutex);
    auto it = m_databases[db].find(key);
    if (it == m_databases[db].end() || it->second.expire_time == 0)
        return -1;
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    return std::max<int>(it->second.expire_time - now, -1);
}

bool Storage::set(size_t db, const std::string &key, const std::string &value)
{
    std::unique_lock lock(m_mutex);
    m_databases[db][key] = Value{ValueType::STRING, value};
    return true;
}

std::optional<std::string> Storage::get(size_t db, const std::string &key)
{
    std::shared_lock lock(m_mutex);
    auto it = m_databases[db].find(key);
    if (it == m_databases[db].end() || it->second.type != ValueType::STRING || isExpired(it->second))
        return std::nullopt;
    return std::get<std::string>(it->second.data);
}

int Storage::incr(size_t db, const std::string &key)
{
    std::unique_lock lock(m_mutex);
    auto &store = m_databases[db];

    if (!store.contains(key) || store[key].type != ValueType::STRING)
        store[key] = Value{ValueType::STRING, "0"};

    auto &val = std::get<std::string>(store[key].data);
    int num = 0;
    try
    {
        num = std::stoi(val);
    }
    catch (...)
    {
        num = 0;
    }

    if (num == INT_MAX)
        throw std::overflow_error("increment overflow");
    num++;
    val = std::to_string(num);
    return num;
}

int Storage::decr(size_t db, const std::string &key)
{
    std::unique_lock lock(m_mutex);
    auto &store = m_databases[db];

    if (!store.contains(key) || store[key].type != ValueType::STRING)
        store[key] = Value{ValueType::STRING, "0"};

    auto &val = std::get<std::string>(store[key].data);
    int num = 0;
    try
    {
        num = std::stoi(val);
    }
    catch (...)
    {
        num = 0;
    }

    if (num == INT_MAX)
        throw std::overflow_error("increment overflow");
    num--;
    val = std::to_string(num);
    return num;
}

bool Storage::append(size_t db, const std::string &key, const std::string &value)
{
    std::unique_lock lock(m_mutex);
    auto &store = m_databases[db];
    if (!store.contains(key) || store[key].type != ValueType::STRING)
        store[key] = Value{ValueType::STRING, ""};
    std::get<std::string>(store[key].data) += value;
    return true;
}

bool Storage::lpush(size_t db, const std::string &key, const std::string &value)
{
    std::unique_lock lock(m_mutex);
    auto &store = m_databases[db];
    if (!store.contains(key))
    {
        store[key] = Value{ValueType::LIST, std::list<std::string>{}};
    }
    if (store[key].type != ValueType::LIST)
        return false;
    std::get<std::list<std::string>>(store[key].data).push_front(value);
    return true;
}

bool Storage::rpush(size_t db, const std::string &key, const std::string &value)
{
    std::unique_lock lock(m_mutex);
    auto &store = m_databases[db];
    if (!store.contains(key))
    {
        store[key] = Value{ValueType::LIST, std::list<std::string>{}};
    }
    if (store[key].type != ValueType::LIST)
        return false;
    std::get<std::list<std::string>>(store[key].data).push_back(value);
    return true;
}

bool Storage::lpop(size_t db, const std::string &key, std::string &value)
{
    std::unique_lock lock(m_mutex);
    auto it = m_databases[db].find(key);
    if (it == m_databases[db].end() || it->second.type != ValueType::LIST || isExpired(it->second))
        return false;
    auto &lst = std::get<std::list<std::string>>(it->second.data);
    if (lst.empty())
        return false;
    value = lst.front();
    lst.pop_front();
    return true;
}

bool Storage::rpop(size_t db, const std::string &key, std::string &value)
{
    std::unique_lock lock(m_mutex);
    auto it = m_databases[db].find(key);
    if (it == m_databases[db].end() || it->second.type != ValueType::LIST || isExpired(it->second))
        return false;
    auto &lst = std::get<std::list<std::string>>(it->second.data);
    if (lst.empty())
        return false;
    value = lst.back();
    lst.pop_back();
    return true;
}

std::vector<std::string> Storage::lrange(size_t db, const std::string &key, int start, int stop)
{
    std::shared_lock lock(m_mutex);
    std::vector<std::string> result;
    auto it = m_databases[db].find(key);
    if (it == m_databases[db].end() || it->second.type != ValueType::LIST || isExpired(it->second))
        return result;

    auto &lst = std::get<std::list<std::string>>(it->second.data);
    int size = static_cast<int>(lst.size());
    if (start < 0)
        start = size + start;
    if (stop < 0)
        stop = size + stop;

    start = std::max(start, 0);
    stop = std::min(stop, size - 1);

    if (start > stop)
        return result;

    int idx = 0;
    for (auto &v : lst)
    {
        if (idx >= start && idx <= stop)
            result.push_back(v);
        idx++;
        if (idx > stop)
            break;
    }
    return result;
}

int Storage::llen(size_t db, const std::string &key)
{
    std::shared_lock lock(m_mutex);
    auto it = m_databases[db].find(key);
    if (it == m_databases[db].end() || it->second.type != ValueType::LIST || isExpired(it->second))
        return 0;
    return static_cast<int>(std::get<std::list<std::string>>(it->second.data).size());
}

bool Storage::sadd(size_t db, const std::string &key, const std::string &member)
{
    std::unique_lock lock(m_mutex);
    auto &store = m_databases[db];
    if (!store.contains(key))
    {
        store[key] = Value{ValueType::SET, std::unordered_set<std::string>{}};
    }
    if (store[key].type != ValueType::SET)
        return false;
    return std::get<std::unordered_set<std::string>>(store[key].data).insert(member).second;
}

bool Storage::srem(size_t db, const std::string &key, const std::string &member)
{
    std::unique_lock lock(m_mutex);
    auto it = m_databases[db].find(key);
    if (it == m_databases[db].end() || it->second.type != ValueType::SET || isExpired(it->second))
        return false;
    return std::get<std::unordered_set<std::string>>(it->second.data).erase(member) > 0;
}

std::unordered_set<std::string> Storage::smembers(size_t db, const std::string &key)
{
    std::shared_lock lock(m_mutex);
    std::unordered_set<std::string> result;
    auto it = m_databases[db].find(key);
    if (it == m_databases[db].end() || it->second.type != ValueType::SET || isExpired(it->second))
        return result;
    return std::get<std::unordered_set<std::string>>(it->second.data);
}

bool Storage::sismember(size_t db, const std::string &key, const std::string &member)
{
    std::shared_lock lock(m_mutex);
    auto it = m_databases[db].find(key);
    if (it == m_databases[db].end() || it->second.type != ValueType::SET || isExpired(it->second))
        return false;
    return std::get<std::unordered_set<std::string>>(it->second.data).count(member) > 0;
}

int Storage::scard(size_t db, const std::string &key)
{
    std::shared_lock lock(m_mutex);
    auto it = m_databases[db].find(key);
    if (it == m_databases[db].end() || it->second.type != ValueType::SET || isExpired(it->second))
        return 0;
    return static_cast<int>(std::get<std::unordered_set<std::string>>(it->second.data).size());
}

std::unordered_set<std::string> Storage::sunion(size_t db, const std::vector<std::string> &keys)
{
    std::shared_lock lock(m_mutex);
    std::unordered_set<std::string> result;
    for (const auto &key : keys)
    {
        auto it = m_databases[db].find(key);
        if (it != m_databases[db].end() && it->second.type == ValueType::SET && !isExpired(it->second))
        {
            auto &members = std::get<std::unordered_set<std::string>>(it->second.data);
            result.insert(members.begin(), members.end());
        }
    }
    return result;
}

std::unordered_set<std::string> Storage::sinter(size_t db, const std::vector<std::string> &keys)
{
    std::shared_lock lock(m_mutex);
    std::unordered_set<std::string> result;
    bool first = true;
    for (const auto &key : keys)
    {
        auto it = m_databases[db].find(key);
        if (it == m_databases[db].end() || it->second.type != ValueType::SET || isExpired(it->second))
        {
            return {};
        }
        auto &members = std::get<std::unordered_set<std::string>>(it->second.data);
        if (first)
        {
            result = members;
            first = false;
        }
        else
        {
            std::unordered_set<std::string> temp;
            for (const auto &m : members)
            {
                if (result.count(m))
                    temp.insert(m);
            }
            result = std::move(temp);
        }
    }
    return result;
}

bool Storage::rename(size_t db, const std::string &old_key, const std::string &new_key)
{
    std::unique_lock lock(m_mutex);
    auto it = m_databases[db].find(old_key);
    if (it == m_databases[db].end())
        return false;
    m_databases[db][new_key] = it->second;
    m_databases[db].erase(it);
    return true;
}