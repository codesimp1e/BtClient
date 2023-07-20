#include "TrackerConnect.h"
#include "torrent.h"
#include <boost/beast/core/stream_traits.hpp>
#include <fstream>
#include <functional>
#include <iostream>

TrackerConnect::TrackerConnect(const ReqInfo &req_info, Torrent *torrent,
                               asio::io_context &ioc, ssl::context &ctx)
    : m_req_info(req_info), m_torrent(torrent), m_tcp_stream(ioc),
      m_ssl_stream(ioc, ctx), m_resolver(asio::make_strand(ioc)) {}

void TrackerConnect::Start() {
  if (m_req_info.m_https) {
    if (!SSL_set_tlsext_host_name(m_ssl_stream.native_handle(),
                                  m_req_info.m_host.data())) {
      beast::error_code ec{static_cast<int>(::ERR_get_error()),
                           asio::error::get_ssl_category()};
      std::cerr << ec.message() << "\n";
      m_torrent->DelConnect(m_req_info.m_url);
      return;
    };
  }
  m_resolver.async_resolve(m_req_info.m_host, m_req_info.m_port,
                           std::bind(&TrackerConnect::OnFindPeersResolve, this,
                                     shared_from_this(), std::placeholders::_1,
                                     std::placeholders::_2));
}

void TrackerConnect::OnFindPeersResolve(std::shared_ptr<TrackerConnect>,
                                        beast::error_code ec,
                                        tcp::resolver::results_type results) {
  if (ec) {
    m_torrent->DelConnect(m_req_info.m_url);
    std::cout << "OnFindPeersResolve err:" << ec.message() << std::endl;
    return;
  }
  if (m_req_info.m_https) {
    beast::get_lowest_layer(m_ssl_stream)
        .async_connect(results,
                       std::bind(&TrackerConnect::OnFindPeersConnect, this,
                                 shared_from_this(), std::placeholders::_1,
                                 std::placeholders::_2));
    return;
  }
  m_tcp_stream.async_connect(
      results,
      std::bind(&TrackerConnect::OnFindPeersConnect, this, shared_from_this(),
                std::placeholders::_1, std::placeholders::_2));
}

void TrackerConnect::OnFindPeersConnect(
    std::shared_ptr<TrackerConnect>, beast::error_code const &ec,
    tcp::resolver::results_type::endpoint_type endpoint) {
  boost::ignore_unused(endpoint);
  if (ec) {
    m_torrent->DelConnect(m_req_info.m_url);
    std::cout << "OnFindPeersConnect err:" << ec.message() << std::endl;
    return;
  }

  m_req = {http::verb::get, m_req_info.m_target, 11};
  m_req.set(http::field::host, m_req_info.m_host);
  m_req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

  if (m_req_info.m_https) {
    m_ssl_stream.async_handshake(
        ssl::stream_base::client,
        std::bind(&TrackerConnect::OnFindPeersHandshake, this,
                  shared_from_this(), std::placeholders::_1));
    return;
  }
  // Send the HTTP request to the remote host
  http::async_write(m_tcp_stream, m_req,
                    std::bind(&TrackerConnect::OnFindPeersWrite, this,
                              shared_from_this(), std::placeholders::_1,
                              std::placeholders::_2));
}

void TrackerConnect::OnFindPeersHandshake(std::shared_ptr<TrackerConnect>,
                                          beast::error_code ec) {
  if (ec) {
    m_torrent->DelConnect(m_req_info.m_url);
    std::cout << "OnFindPeersHandshake err:" << ec.message() << std::endl;
    return;
  }
  http::async_write(m_ssl_stream, m_req,
                    std::bind(&TrackerConnect::OnFindPeersWrite, this,
                              shared_from_this(), std::placeholders::_1,
                              std::placeholders::_2));
}

// 向tracker服务器请求数据
void TrackerConnect::OnFindPeersWrite(std::shared_ptr<TrackerConnect> self,
                                      beast::error_code const &ec,
                                      size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);
  if (ec) {
    m_torrent->DelConnect(m_req_info.m_url);
    std::cout << "HandleFindPeersWrite err:" << ec.message() << std::endl;
    return;
  }
  if (m_req_info.m_https) {
    http::async_read(m_ssl_stream, m_buffer, m_res,
                     std::bind(&TrackerConnect::OnFindPeersRead, this,
                               shared_from_this(), std::placeholders::_1,
                               std::placeholders::_2));
    return;
  }
  http::async_read(m_tcp_stream, m_buffer, m_res,
                   std::bind(&TrackerConnect::OnFindPeersRead, this,
                             shared_from_this(), std::placeholders::_1,
                             std::placeholders::_2));
}

// 接收服务器返回数据
void TrackerConnect::OnFindPeersRead(std::shared_ptr<TrackerConnect> self,
                                     beast::error_code const &ec,
                                     size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);
  if (ec) {
    m_torrent->DelConnect(m_req_info.m_url);
    std::cout << "HandleFindPeersRead err:" << ec.message() << std::endl;
    return;
  }

  // std::cout << m_res << std::endl;

  auto &body = m_res.body();
  auto body_str = boost::beast::buffers_to_string(body.data());
  // std::cout << body_str << std::endl;

  m_torrent->m_peer.Parse(body_str);
  // std::ofstream fout("peer.tt");
  // fout<<body_str;

  // Gracefully close the socket
  beast::error_code ec_shutdown;
  m_tcp_stream.socket().shutdown(tcp::socket::shutdown_both, ec_shutdown);
  m_torrent->DelConnect(m_req_info.m_url);
}