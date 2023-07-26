#pragma once
#include <boost/url.hpp>
#include <cstdint>
#include <sstream>
#include <string>

std::string UrlEncode(const std::string &);

struct ReqInfo {
  ReqInfo(const std::string &host, const std::string &port,
          const std::string &target)
      : m_host(host), m_port(port), m_target(target){};
  ReqInfo(const std::string &url_full) {
    auto url_result = boost::urls::parse_uri(url_full);
    if (!url_result) {
      return;
    }
    auto url = *url_result;
    m_host = std::string(url.encoded_host());
    m_port = url.port();
    m_target = std::string(url.encoded_target());
    m_https = (url.scheme() == "https");
    if (m_port.empty()) {
      m_port = m_https ? "443" : "80";
    }
    m_url = url_full;
  }
  std::string m_host;
  std::string m_port;
  std::string m_target;
  std::string m_url;
  bool m_https;
};

std::string to_hex(const uint8_t *data, int len);