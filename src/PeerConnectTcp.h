#pragma once

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/system/detail/error_code.hpp>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

namespace asio = boost::asio;
using tcp = boost::asio::ip::tcp;

class Torrent;

class PeerConnectTcp : public std::enable_shared_from_this<PeerConnectTcp> {
public:
  class HandshakePacket {
  public:
    char m_pstr[19];
    char m_info_hash[20];
    char m_peer_id[20];
    HandshakePacket(const uint8_t *data);
    HandshakePacket(const std::string &info_hash, const std::string &peer_id);
    void SetData(const uint8_t *data);
    void GetData(uint8_t *data);
  };
  PeerConnectTcp(asio::io_context &, const std::string &, unsigned short,
                 Torrent *);
  void Start();

private:
  void HandleResolve(std::shared_ptr<PeerConnectTcp>, boost::system::error_code,
                     tcp::resolver::results_type);
  void HandleConnect(std::shared_ptr<PeerConnectTcp>, boost::system::error_code,
                     tcp::endpoint);
  void HandleHandshakeWrite(std::shared_ptr<PeerConnectTcp>,
                            boost::system::error_code, size_t);
  void HandleHandshakeRead(std::shared_ptr<PeerConnectTcp>,
                           boost::system::error_code, size_t);
  tcp::socket m_socket;
  tcp::resolver m_resolver;

  std::string m_addr;
  std::string m_port;

  uint8_t handshake_data[68];
  std::string m_info_hash;
  std::string m_id;
  Torrent *m_torrent;
  std::string m_key;
};