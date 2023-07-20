#include "bdecode.h"
#include <iostream>

BDecoder::BDecoder() {}

BDecoder::Value::ptr BDecoder::GetRoot() { return m_value; }

void BDecoder::Decode(std::istream &stream) {
  m_in << stream.rdbuf();
  m_value = Parse();
  // std::ofstream fout("debian_copy.torrent");
  // fout.write(m_value->raw_value.c_str(), m_value->raw_value.size());
  // fout.close();
  m_in.clear();
}

bool BDecoder::IsEnd() {
  char t;
  m_in >> t;
  if (t != 'e') {
    return false;
  }
  return true;
}

bool BDecoder::ParseInt(long long &num) {
  try {
    m_in >> num;
    return IsEnd();
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return false;
  }
}
/// @brief 解析字符串类型 <字符串长度>:<字符串>
/// @param str
/// @return 是否解析成功
bool BDecoder::ParseString(std::string &str) {
  try {
    int str_len;
    m_in >> str_len;
    if (str_len < 0) {
      return false;
    }
    char t;
    m_in >> t;
    if (t != ':') {
      return false;
    }
    if (str_len == 0) {
      str = "";
      return true;
    }

    char str_buf[str_len];
    m_in.read(str_buf, str_len);
    str = std::string(str_buf, str_len);

    return true;
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return false;
  }
}

BDecoder::Value::ptr BDecoder::Parse() {
  char t;
  auto pos = m_in.tellg();
  try {
    m_in >> t;
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return nullptr;
  }
  // 整型
  if (t == 'i') {
    long long num;
    if (!ParseInt(num)) {
      return nullptr;
    }
    return std::dynamic_pointer_cast<Value>(IntValue::ptr(new IntValue(num)));
  }
  // 字符串
  if (t >= '0' && t <= '9') {
    m_in.seekg(pos);
    std::string s;
    if (!ParseString(s)) {
      return nullptr;
    }
    return std::dynamic_pointer_cast<Value>(
        StringValue::ptr(new StringValue(s)));
  }
  // 列表
  if (t == 'l') {
    auto list_value = ListValue::ptr(new ListValue());
    std::stringstream ss;
    ss << 'l';
    while (true) {
      auto pos_l = m_in.tellg();
      try {
        if (IsEnd()) {
          break;
        }
      } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return nullptr;
      }

      m_in.seekg(pos_l);

      auto result = Parse();
      if (!result) {
        return nullptr;
      }
      list_value->value.push_back(result);
      ss << result->raw_value;
    }
    ss << 'e';
    list_value->raw_value = ss.str();
    return std::dynamic_pointer_cast<Value>(list_value);
  }

  // 字典
  if (t == 'd') {
    auto dict_value = DictValue::ptr(new DictValue());
    std::stringstream ss;
    ss << 'd';
    while (true) {
      auto pos_l = m_in.tellg();
      try {
        if (IsEnd()) {
          break;
        }
      } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return nullptr;
      }

      m_in.seekg(pos_l);
      std::string key;
      if (!ParseString(key)) {

        return nullptr;
      }

      ss << key.size() << ':' << key;

      auto result = Parse();
      if (!result) {
        return nullptr;
      }
      ss << result->raw_value;
      dict_value->value[key] = result;
    }
    ss << 'e';
    dict_value->raw_value = ss.str();
    return std::dynamic_pointer_cast<Value>(dict_value);
  }
  return nullptr;
}
