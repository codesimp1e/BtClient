#include "PeerConnect.h"
#include <boost/asio/detail/socket_ops.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/registered_buffer.hpp>
#include <boost/asio/write.hpp>
#include <cstdint>

#include <functional>
#include <iostream>
#include <string>

PeerConnect::PeerConnect(asio::io_context &ioc, const std::string &addr,
                         unsigned short port)
    : m_socket(ioc), m_endpoint(asio::ip::address::from_string(addr), port) {}

void PeerConnect::Start() {
  const char *header = m_send_pkg.header.GetData();
  // 建立udp连接
  m_socket.async_connect(m_endpoint,
                         std::bind(&PeerConnect::HandleConnect, this,
                                   shared_from_this(), std::placeholders::_1,
                                   std::placeholders::_2));
}
void PeerConnect::HandleConnect(std::shared_ptr<PeerConnect> self,
                                boost::system::error_code ec,
                                udp::endpoint ep) {
  if (ec) {
    std::cout << "HandleConnect err:" << ec.message() << std::endl;
    return;
  }

  // 发送SYNC，建立uTP连接
  asio::async_write(
      m_socket, asio::buffer(m_send_pkg.header.GetData(), UTP_HEADER_LENGTH),
      std::bind(&PeerConnect::HandleEstablishWrite, this, shared_from_this(),
                std::placeholders::_1, std::placeholders::_2));
}

void PeerConnect::HandleEstablishWrite(std::shared_ptr<PeerConnect> self,
                                       boost::system::error_code ec,
                                       size_t size) {
  if (ec) {
    std::cout << "HandleConnect err:" << ec.message() << std::endl;
    return;
  }

  // 接收对端的应答
  asio::async_read(
      m_socket, asio::buffer(m_recv_header_data, UTP_HEADER_LENGTH),
      std::bind(&PeerConnect::HandleEstablishRead, this, shared_from_this(),
                std::placeholders::_1, std::placeholders::_2));
}

void PeerConnect::HandleEstablishRead(std::shared_ptr<PeerConnect>,
                                      boost::system::error_code ec,
                                      size_t size) {
  if (ec) {
    std::cout << "HandleEstablishRead err:" << ec.message() << std::endl;
    return;
  }
  m_recv_pkg.header.SetData(m_recv_header_data);
  std::cout << m_recv_pkg.header.ToString() << std::endl;
}