#include "PeerConnectUtp.h"
#include <boost/asio/connect.hpp>
#include <boost/asio/detail/socket_ops.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/registered_buffer.hpp>
#include <boost/asio/write.hpp>
#include <cstdint>

#include <functional>
#include <iostream>
#include <string>

PeerConnectUtp::PeerConnectUtp(asio::io_context &ioc, const std::string &addr,
                         unsigned short port)
    : m_socket(ioc) {
  udp::resolver resolver(ioc);
  m_endpoint = resolver.resolve(addr, std::to_string(port));
}

void PeerConnectUtp::Start() {
  const char *header = m_send_pkg.header.GetData();
  // 建立udp连接
  asio::async_connect(m_socket, m_endpoint,
                      std::bind(&PeerConnectUtp::HandleConnect, this,
                                shared_from_this(), std::placeholders::_1,
                                std::placeholders::_2));
}
void PeerConnectUtp::HandleConnect(std::shared_ptr<PeerConnectUtp> self,
                                boost::system::error_code ec,
                                udp::endpoint ep) {
  if (ec) {
    std::cout << "HandleConnect err:" << ec.message() << std::endl;
    return;
  }

  // 发送SYNC，建立uTP连接
  m_socket.async_send(
      asio::buffer(m_send_pkg.header.GetData(), UTP_HEADER_LENGTH),
      std::bind(&PeerConnectUtp::HandleEstablishWrite, this, shared_from_this(),
                std::placeholders::_1, std::placeholders::_2));
}

void PeerConnectUtp::HandleEstablishWrite(std::shared_ptr<PeerConnectUtp> self,
                                       boost::system::error_code ec,
                                       size_t size) {
  if (ec) {
    std::cout << "HandleConnect err:" << ec.message() << std::endl;
    return;
  }

  // 接收对端的应答
  m_socket.async_receive(asio::buffer(m_recv_header_data, UTP_HEADER_LENGTH),
                         std::bind(&PeerConnectUtp::HandleEstablishRead, this,
                                   shared_from_this(), std::placeholders::_1,
                                   std::placeholders::_2));
}

void PeerConnectUtp::HandleEstablishRead(std::shared_ptr<PeerConnectUtp>,
                                      boost::system::error_code ec,
                                      size_t size) {
  if (ec) {
    std::cout << "HandleEstablishRead err:" << ec.message() << std::endl;
    return;
  }
  m_recv_pkg.header.SetData(m_recv_header_data);
  std::cout << m_recv_pkg.header.ToString() << std::endl;
}