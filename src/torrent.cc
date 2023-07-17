#include "torrent.h"
#include "bdecode.h"
#include <iostream>
#include <cstring>
#include <sstream>

Torrent::Torrent(const char *file) : m_file(file)
{
    BDecoder decoder(file);
    decoder.Decode();
    std::map<std::string, BDecoder::Value::ptr> maps;
    try
    {
        maps = decoder.Cast<BDecoder::DictValue>();

        auto t = maps.at("announce");
        m_announce = BDecoder::Cast<BDecoder::StringValue>(t);

        t = maps.at("info");
        auto info_map = BDecoder::Cast<BDecoder::DictValue>(t);

        t = info_map.at("name");
        m_info.m_name = BDecoder::Cast<BDecoder::StringValue>(t);

        t = info_map.at("piece length");
        m_info.m_pieces_len = BDecoder::Cast<BDecoder::IntValue>(t);

        t = info_map.at("pieces");
        auto pieces = BDecoder::Cast<BDecoder::StringValue>(t);
        m_info.m_pieces = new char[pieces.size()];
        memcpy(m_info.m_pieces, pieces.c_str(), pieces.size());

        auto it = maps.find("files");
        if (it != maps.end())
        {
            /* code */
        }
        it = maps.find("comment");
        if (it != maps.end())
        {
            m_comment = BDecoder::Cast<BDecoder::StringValue>(it->second);
        }
        it = maps.find("announce-list");
        if (it != maps.end())
        {
            auto a_list = BDecoder::Cast<BDecoder::ListValue>(it->second);
            for (auto &&i : a_list)
            {
                auto url_list = BDecoder::Cast<BDecoder::ListValue>(i);
                std::vector<std::string> url_vec;
                for (auto &&url : url_list)
                {
                    url_vec.emplace_back(BDecoder::Cast<BDecoder::StringValue>(url));
                }
                m_announce_list.emplace_back(url_vec);
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        delete this;
        return;
    }
}

Torrent::~Torrent()
{
    if (m_info.m_pieces)
    {
        delete[] m_info.m_pieces;
        m_info.m_pieces = nullptr;
    }
}

std::string Torrent::ToString()
{
    std::stringstream ss;
    ss << "announce:\n\t" << m_announce
       << "\ncomment:\n\t" << m_comment
       << "\ninfo:\n\t"
       << "name:\n\t\t" << m_info.m_name
       << "\n\tpiece length:\n\t\t" << m_info.m_pieces_len
       << "\nannounce-list:\n\t[";
    for (const auto &i : m_announce_list)
    {
        ss << '[';
        for (const auto &j : i)
        {
            ss << j << ' ';
        }
        ss << "] ";
    }
    ss << ']';
    return ss.str();
}