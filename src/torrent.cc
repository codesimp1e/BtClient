#include "torrent.h"
#include "TrackerConnect.h"
#include "TrackerLists.h"
#include "bdecode.h"
#include "utils.h"
#include <boost/asio/buffer.hpp>
#include <boost/asio/ssl/context.hpp>
#include <cryptopp/cryptlib.h>
#include <cryptopp/files.h>
#include <cryptopp/hex.h>
#include <cryptopp/sha.h>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "ServicePool.h"

namespace ssl = boost::asio::ssl;

Torrent::Torrent(const char *file, int port)
    : m_file(file), m_port(port), m_ctx(ssl::context::tlsv12_client) {
  std::ostringstream os;
  ::srand(::time(0));
  for (size_t i = 0; i < 20; i++) {
    char c = char(rand() % 26 + 'A');
    os << c;
  }
  m_peer_id = os.str();
}

bool Torrent::Parse() {
  BDecoder decoder;
  std::ifstream fin(m_file, std::ios::in);
  decoder.Decode(fin);
  fin.close();
  std::map<std::string, BDecoder::Value::ptr> maps;
  try {
    maps = decoder.Cast<BDecoder::DictValue>();

    auto t = maps.at("announce");
    m_announce = BDecoder::Cast<BDecoder::StringValue>(t);

    t = maps.at("info");
    auto info_map = BDecoder::Cast<BDecoder::DictValue>(t);

    std::stringstream ss;
    CryptoPP::SHA1 hash;
    CryptoPP::HexEncoder encoder(new CryptoPP::FileSink(ss));
    hash.Update((const CryptoPP::byte *)(t->raw_value.data()),
                t->raw_value.size());
    char digest[hash.DigestSize()];
    hash.Final((CryptoPP::byte *)digest);
    CryptoPP::StringSource(digest, true, new CryptoPP::Redirector(encoder));
    m_info_hash = ss.str().substr(0, hash.DigestSize() * 2);
    m_info_url = UrlEncode(std::string(digest, hash.DigestSize()));

    t = info_map.at("name");
    m_info.m_name = BDecoder::Cast<BDecoder::StringValue>(t);

    t = info_map.at("piece length");
    m_info.m_pieces_len = BDecoder::Cast<BDecoder::IntValue>(t);

    t = info_map.at("pieces");
    auto pieces = BDecoder::Cast<BDecoder::StringValue>(t);
    m_info.m_pieces = new char[pieces.size()];
    memcpy(m_info.m_pieces, pieces.c_str(), pieces.size());

    auto it = info_map.find("length");
    if (it != info_map.end()) {
      m_info.length = BDecoder::Cast<BDecoder::IntValue>(it->second);
    }

    it = maps.find("files");
    if (it != maps.end()) {
      /* code */
    }
    it = maps.find("comment");
    if (it != maps.end()) {
      m_comment = BDecoder::Cast<BDecoder::StringValue>(it->second);
    }
    it = maps.find("announce-list");
    if (it != maps.end()) {
      auto a_list = BDecoder::Cast<BDecoder::ListValue>(it->second);
      for (auto &&i : a_list) {
        auto url_list = BDecoder::Cast<BDecoder::ListValue>(i);
        std::vector<std::string> url_vec;
        for (auto &&url : url_list) {
          url_vec.emplace_back(BDecoder::Cast<BDecoder::StringValue>(url));
        }
        m_announce_list.emplace_back(url_vec);
      }
    }
    m_query = Query();
    return true;
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return false;
  }
}

void Torrent::FindPeers() {

  auto &ioc = ServicePool::GetInstace().GetService();

  std::ifstream fin("/etc/ssl/certs/ca-certificates.crt");
  std::stringstream ss;
  ss << fin.rdbuf();
  std::string cert = ss.str();
  m_ctx.add_certificate_authority(asio::buffer(cert.data(), cert.size()));
  m_ctx.set_verify_mode(ssl::verify_peer);

  auto full_url = m_announce + m_query;
  auto connect = std::shared_ptr<TrackerConnect>(
      new TrackerConnect(full_url, this, ioc, m_ctx));
  m_connects[full_url] = connect;
  connect->Start();

  // announce_list 中查询
  for (const auto &a_list : m_announce_list) {
    for (const auto &announce : a_list) {
      std::cout << announce << std::endl;
      full_url = announce + m_query;
      auto it = m_connects.find(full_url);
      if (it != m_connects.end()) {
        continue;
      }
      auto connect = std::shared_ptr<TrackerConnect>(
          new TrackerConnect(announce + m_query, this, ioc, m_ctx));
      m_connects[full_url] = connect;
      connect->Start();
    }
  }

  // tracker_list 中查询
  for (const auto &announce : TRACKER_LIST) {
    std::cout << announce << std::endl;
    auto it = m_connects.find(announce);
    if (it != m_connects.end()) {
      continue;
    }
    auto connect = std::shared_ptr<TrackerConnect>(
        new TrackerConnect(announce + m_query, this, ioc, m_ctx));
    m_connects[full_url] = connect;
    connect->Start();
  }
}

std::string Torrent::Query() {
  std::stringstream ss;
  ss << "?info_hash=" << m_info_url << "&peer_id=" << m_peer_id
     << "&port=" << m_port << "&uploaded=" << 0 << "&downloaded=" << 0
     << "&left=" << m_info.length << "&compact=" << 1;
  return ss.str();
}
std::string Torrent::GetQuery() { return m_query; }

void Torrent::DelConnect(const std::string &url) { m_connects.erase(url); }

Torrent::~Torrent() {
  if (m_info.m_pieces) {
    delete[] m_info.m_pieces;
    m_info.m_pieces = nullptr;
  }
}

std::string Torrent::ToString() {
  std::stringstream ss;
  ss << "announce:\n\t" << m_announce << "\ncomment:\n\t" << m_comment
     << "\ninfo_hash:\n\t" << m_info_hash << "\nm_info_url:\n\t" << m_info_url
     << "\ninfo:\n\t"
     << "length:\n\t\t" << m_info.length << "\n\tname:\n\t\t" << m_info.m_name
     << "\n\tpiece length:\n\t\t" << m_info.m_pieces_len << "\nannounce-list("
     << m_announce_list.size() << "):\n\t[";
  for (const auto &i : m_announce_list) {
    ss << '[';
    for (const auto &j : i) {
      ss << j << ' ';
    }
    ss << "] ";
  }
  ss << ']' << std::endl;
  return ss.str();
}

TrackerEvent::TrackerEvent(const char *req, size_t size,
                           std::shared_ptr<Torrent> torrent)
    : m_torrent(torrent) {
  m_req = new char[size];
  memcpy(m_req, req, size);
}

TrackerEvent::~TrackerEvent() {
  if (m_req) {
    delete[] m_req;
  }
  m_req = nullptr;
}