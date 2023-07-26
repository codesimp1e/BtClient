#include "PeerConnectTcp.h"
#include "torrent.h"
#include "utils.h"
#include <boost/asio/buffer.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <cstdint>
#include <cstring>
#include <functional>
#include <iostream>
#include <string>

PeerConnectTcp::PeerConnectTcp(asio::io_context &ioc, const std::string &addr,
                               unsigned short port, Torrent *torrent)
    : m_socket(ioc), m_resolver(ioc), m_info_hash(torrent->m_info_hash_raw),
      m_id(torrent->m_peer_id), m_torrent(torrent), m_addr(addr),
      m_port(std::to_string(port)) {
  m_key = addr + std::to_string(port);
}

PeerConnectTcp::HandshakePacket::HandshakePacket(const std::string &info_hash,
                                                 const std::string &peer_id) {
  memcpy(m_pstr, "BitTorrent protocol", 19);
  memcpy(m_info_hash, info_hash.c_str(), 20);
  memcpy(m_peer_id, peer_id.c_str(), 20);
}

PeerConnectTcp::HandshakePacket::HandshakePacket(const uint8_t *data) {
  SetData(data);
}

void PeerConnectTcp::HandshakePacket::SetData(const uint8_t *data) {
  memcpy(m_pstr, data + 1, 19);
  memcpy(m_info_hash, data + 28, 20);
  memcpy(m_peer_id, data + 48, 20);
}

void PeerConnectTcp::HandshakePacket::GetData(uint8_t *data) {
  memset(data, 19, 1);
  memcpy(data + 1, m_pstr, 19);
  memset(data + 20, 0, 8);
  memcpy(data + 28, m_info_hash, 20);
  memcpy(data + 48, m_peer_id, 20);
}

void PeerConnectTcp::Start() {
  m_resolver.async_resolve(m_addr, m_port,
                           std::bind(&PeerConnectTcp::HandleResolve, this,
                                     shared_from_this(), std::placeholders::_1,
                                     std::placeholders::_2));
}

void PeerConnectTcp::HandleResolve(std::shared_ptr<PeerConnectTcp>,
                                   boost::system::error_code ec,
                                   tcp::resolver::results_type endpoint) {
  if (ec) {
    std::cerr << "HandleResolve err:" << ec.message() << std::endl;
    m_torrent->DelPeerConnectTcp(m_key);
    return;
  }
  asio::async_connect(m_socket, endpoint,
                      std::bind(&PeerConnectTcp::HandleConnect, this,
                                shared_from_this(), std::placeholders::_1,
                                std::placeholders::_2));
}

void PeerConnectTcp::HandleConnect(std::shared_ptr<PeerConnectTcp>,
                                   boost::system::error_code ec,
                                   tcp::endpoint) {
  if (ec) {
    std::cerr << "HandleConnect err:" << ec.message() << std::endl;
    m_torrent->DelPeerConnectTcp(m_key);
    return;
  }

  HandshakePacket hpkg(m_torrent->m_info_hash_raw, m_torrent->m_peer_id);
  hpkg.GetData(handshake_data);

  asio::async_write(m_socket, asio::buffer(handshake_data, 68),
                    std::bind(&PeerConnectTcp::HandleHandshakeRead, this,
                              shared_from_this(), std::placeholders::_1,
                              std::placeholders::_2));
}
void PeerConnectTcp::HandleHandshakeWrite(std::shared_ptr<PeerConnectTcp>,
                                          boost::system::error_code ec,
                                          size_t) {
  if (ec) {
    std::cerr << "HandleHandshakeWrite err:" << ec.message() << std::endl;
    m_torrent->DelPeerConnectTcp(m_key);
    return;
  }
  memset(handshake_data, 0, 68);
  asio::async_read(m_socket, asio::buffer(handshake_data, 68),
                   std::bind(&PeerConnectTcp::HandleHandshakeRead, this,
                             shared_from_this(), std::placeholders::_1,
                             std::placeholders::_2));
}
void PeerConnectTcp::HandleHandshakeRead(std::shared_ptr<PeerConnectTcp>,
                                         boost::system::error_code ec, size_t) {
  if (ec) {
    std::cerr << "HandleHandshakeRead err:" << ec.message() << std::endl;
    m_torrent->DelPeerConnectTcp(m_key);
    return;
  }

  HandshakePacket hpkg(handshake_data);

  // auto m_hash = to_hex((uint8_t *)m_info_hash.c_str(), 20);
  // auto recv_hash = to_hex((uint8_t *)hpkg.m_info_hash, 20);
  // std::cout << m_hash << ' ' << m_id << ' ' << recv_hash << ' '
  //           << std::string(hpkg.m_peer_id, 20) << std::endl;

  m_torrent->DelPeerConnectTcp(m_key);
}