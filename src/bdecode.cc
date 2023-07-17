#include "bdecode.h"
#include <iostream>

BDecoder::BDecoder(const char *file) : m_file(file), m_value(nullptr)
{
    // std::cout << m_file << std::endl;
}

void BDecoder::Decode()
{
    m_fin.open(m_file, std::ios::in);
    m_value = Parse();
    m_fin.close();
}

bool BDecoder::IsEnd()
{
    char t;
    m_fin >> t;
    if (t != 'e')
    {
        return false;
    }
    return true;
}

bool BDecoder::ParseInt(long long &num)
{
    try
    {
        m_fin >> num;
        return IsEnd();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
}
/// @brief 解析字符串类型 <字符串长度>:<字符串>
/// @param str
/// @return 是否解析成功
bool BDecoder::ParseString(std::string &str)
{
    try
    {
        int str_len;
        m_fin >> str_len;
        if (str_len < 0)
        {
            return false;
        }
        if (str_len == 0)
        {
            str = "";
            return true;
        }
        char t;
        m_fin >> t;
        if (t != ':')
        {
            return false;
        }

        char str_buf[str_len];
        m_fin.read(str_buf, str_len);
        str = std::string(str_buf, str_len);

        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
}

BDecoder::Value::ptr BDecoder::Parse()
{
    char t;
    auto pos = m_fin.tellg();
    try
    {
        m_fin >> t;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return nullptr;
    }
    // 整型
    if (t == 'i')
    {
        long long num;
        if (!ParseInt(num))
        {
            return nullptr;
        }
        // std::cout << num << ' ';
        return std::dynamic_pointer_cast<Value>(IntValue::ptr(new IntValue(num)));
    }
    // 字符串
    if (t >= '0' && t <= '9')
    {
        m_fin.seekg(pos);
        std::string s;
        if (!ParseString(s))
        {
            return nullptr;
        }
        // std::cout << s << ' ';
        return std::dynamic_pointer_cast<Value>(StringValue::ptr(new StringValue(s)));
    }
    // 列表
    if (t == 'l')
    {
        auto list_value = ListValue::ptr(new ListValue());
        while (true)
        {
            auto pos_l = m_fin.tellg();
            try
            {
                if (IsEnd())
                {
                    break;
                }
            }
            catch (const std::exception &e)
            {
                std::cerr << e.what() << '\n';
                return nullptr;
            }

            m_fin.seekg(pos_l);

            auto result = Parse();
            if (!result)
            {
                return nullptr;
            }
            list_value->value.push_back(result);
        }
        return std::dynamic_pointer_cast<Value>(list_value);
    }

    // 字典
    if (t == 'd')
    {
        auto dict_value = DictValue::ptr(new DictValue());
        while (true)
        {
            auto pos_l = m_fin.tellg();
            try
            {
                if (IsEnd())
                {
                    break;
                }
            }
            catch (const std::exception &e)
            {
                std::cerr << e.what() << '\n';
                return nullptr;
            }

            m_fin.seekg(pos_l);
            std::string key;
            if (!ParseString(key))
            {

                return nullptr;
            }
            // std::cout << key << ":";
            auto result = Parse();
            if (!result)
            {
                return nullptr;
            }
            dict_value->value[key] = result;
            // std::cout << std::endl;
        }
        return std::dynamic_pointer_cast<Value>(dict_value);
    }
    return nullptr;
}
