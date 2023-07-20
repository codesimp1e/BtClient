#pragma once
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>
#include <boost/url/parse.hpp>
#include "utils.h"

namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http = beast::http;   // from <boost/beast/http.hpp>
namespace asio = boost::asio;   // from <boost/asio.hpp>
namespace ssl = asio::ssl;
using tcp = asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

class Torrent;

class TrackerConnect : public std::enable_shared_from_this<TrackerConnect> {

public:


  TrackerConnect(const ReqInfo &, Torrent *, asio::io_context &ioc,
                 ssl::context &ctx);
  void Start();

private:
  beast::tcp_stream m_tcp_stream;
  beast::ssl_stream<beast::tcp_stream> m_ssl_stream;

  http::response<http::dynamic_body> m_res;
  http::request<http::string_body> m_req;
  // beast::flat_buffer* m_buffer;
  tcp::resolver m_resolver;
  Torrent *m_torrent;
  beast::flat_buffer m_buffer;

  ReqInfo m_req_info;

  void OnFindPeersRead(std::shared_ptr<TrackerConnect>,
                       beast::error_code const &, size_t bytes_transferred);
  void OnFindPeersWrite(std::shared_ptr<TrackerConnect>,
                        beast::error_code const &, size_t bytes_transferred);
  void OnFindPeersConnect(std::shared_ptr<TrackerConnect>,
                          beast::error_code const &,
                          tcp::resolver::results_type::endpoint_type);
  void OnFindPeersResolve(std::shared_ptr<TrackerConnect>, beast::error_code ec,
                          tcp::resolver::results_type results);
  void OnFindPeersHandshake(std::shared_ptr<TrackerConnect>,
                            beast::error_code ec);
};