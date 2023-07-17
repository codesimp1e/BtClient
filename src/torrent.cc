#include "torrent.h"
#include "bdecode.h"
#include <iostream>
#include <cstring>
#include <sstream>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/url/parse.hpp>
#include <cryptopp/cryptlib.h>
#include <cryptopp/sha.h>
#include <cryptopp/hex.h>
#include <cryptopp/files.h>

namespace urls = boost::urls;
namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http = beast::http;   // from <boost/beast/http.hpp>
namespace net = boost::asio;    // from <boost/asio.hpp>
using tcp = net::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

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

        std::stringstream ss;
        CryptoPP::SHA1 hash;
        CryptoPP::HexEncoder encoder(new CryptoPP::FileSink(ss));
        hash.Update((const CryptoPP::byte *)(t->raw_value.data()), t->raw_value.size());
        char digest[hash.DigestSize()];
        hash.Final((CryptoPP::byte *)digest);
        CryptoPP::StringSource(digest, true, new CryptoPP::Redirector(encoder));
        m_info_hash = ss.str().substr(0, hash.DigestSize() * 2);

        t = info_map.at("name");
        m_info.m_name = BDecoder::Cast<BDecoder::StringValue>(t);

        t = info_map.at("piece length");
        m_info.m_pieces_len = BDecoder::Cast<BDecoder::IntValue>(t);

        t = info_map.at("pieces");
        auto pieces = BDecoder::Cast<BDecoder::StringValue>(t);
        m_info.m_pieces = new char[pieces.size()];
        memcpy(m_info.m_pieces, pieces.c_str(), pieces.size());

        auto it = info_map.find("length");
        if (it != info_map.end())
        {
            m_info.length = BDecoder::Cast<BDecoder::IntValue>(it->second);
        }

        it = maps.find("files");
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

int Torrent::FindPeers()
{

    auto url_result = urls::parse_uri(m_announce);
    if (!url_result)
    {
        return -1;
    }
    auto url = *url_result;
    std::cout << url.host() << std::endl
              << url.port() << std::endl
              << url.encoded_target() << std::endl;

    net::io_context ioc;

    tcp::resolver resolver(ioc);
    beast::tcp_stream tcp_stream(ioc);

    auto const results = resolver.resolve(url.host(), url.port());
    tcp_stream.connect(results);
    http::request<http::string_body> req{
        http::verb::get, url.encoded_target(), 11};
    req.set(http::field::host, url.host());
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    return -1;
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
       << "\ninfo_hash:\n\t" << m_info_hash
       << "\ninfo:\n\t"
       << "length:\n\t\t" << m_info.length
       << "\n\tname:\n\t\t" << m_info.m_name
       << "\n\tpiece length:\n\t\t" << m_info.m_pieces_len
       << "\nannounce-list(" << m_announce_list.size() << "):\n\t[";
    for (const auto &i : m_announce_list)
    {
        ss << '[';
        for (const auto &j : i)
        {
            ss << j << ' ';
        }
        ss << "] ";
    }
    ss << ']' << std::endl;
    return ss.str();
}