#pragma once

#include "UtpPacket.h"
#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/system/detail/error_code.hpp>
#include <cstddef>
#include <cstdint>
#include <memory>

namespace asio = boost::asio;
using udp = boost::asio::ip::udp;

class PeerConnectUtp : public std::enable_shared_from_this<PeerConnectUtp> {
public:
  PeerConnectUtp(asio::io_context &, const std::string &, unsigned short);
  void Start();

private:
  void HandleConnect(std::shared_ptr<PeerConnectUtp>, boost::system::error_code,
                     udp::endpoint);
  void HandleEstablishWrite(std::shared_ptr<PeerConnectUtp>,
                            boost::system::error_code, size_t);
  void HandleEstablishRead(std::shared_ptr<PeerConnectUtp>,
                           boost::system::error_code, size_t);
  udp::socket m_socket;
  udp::resolver::results_type m_endpoint;

  UtpPacket m_send_pkg;
  UtpPacket m_recv_pkg;
  uint8_t m_recv_header_data[UTP_HEADER_LENGTH];
};