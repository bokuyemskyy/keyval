#pragma once

#include <optional>
#include <string>
#include <unordered_map>
class Database
{
public:
  void set(const std::string &key, const std::string &value)
  {
    storage[key] = value;
  }
  [[nodiscard]] std::optional<std::string> get(const std::string &key) const
  {
    auto it = storage.find(key);
    if (it != storage.end())
      return it->second;
    return std::nullopt;
  }
  bool del(const std::string &key) { return storage.erase(key) != 0; }
  bool empty() { return storage.empty(); }
  std::string keys()
  {
    std::string result;
    for (const auto &[key, value] : storage)
    {
      result += key + "\n";
    }
    return result;
  }
  void flushall() { storage.clear(); }

private:
  std::unordered_map<std::string, std::string> storage;
};