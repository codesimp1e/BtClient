#pragma once
#include <fstream>
#include <list>
#include <map>
#include <memory>
#include <sstream>

class BDecoder
{
  friend class Value;

public:
  BDecoder(const char *);
  class Value
  {
  public:
    using ptr = std::shared_ptr<Value>;
    virtual ~Value() {}
    std::string raw_value;
  };
  class StringValue : public Value
  {
  public:
    using ptr = std::shared_ptr<StringValue>;
    StringValue(std::string str) : value(str)
    {
      std::stringstream ss;
      ss << str.size() << ':' << str;
      raw_value = ss.str();
    }
    std::string value;
  };
  class IntValue : public Value
  {
  public:
    using ptr = std::shared_ptr<IntValue>;
    IntValue(long long num) : value(num)
    {
      std::stringstream ss;
      ss << 'i' << num << 'e';
      raw_value = ss.str();
    }
    long long value;
  };
  class ListValue : public Value
  {
  public:
    using ptr = std::shared_ptr<ListValue>;
    std::list<Value::ptr> value;
  };
  class DictValue : public Value
  {
  public:
    using ptr = std::shared_ptr<DictValue>;
    std::map<std::string, Value::ptr> value;
  };
  void Decode();

  template <class T>
  decltype(std::declval<T>().value) Cast()
  {
    return std::dynamic_pointer_cast<T>(m_value)->value;
  }

  template <class T>
  static decltype(std::declval<T>().value) Cast(Value::ptr value)
  {
    return std::dynamic_pointer_cast<T>(value)->value;
  }

private:
  std::ifstream m_fin;
  std::string m_file;
  Value::ptr m_value;
  bool IsEnd();
  bool ParseInt(long long &num);
  bool ParseString(std::string &str);
  Value::ptr Parse();
};