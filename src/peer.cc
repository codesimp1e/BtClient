#include "peer.h"
#include "arpa/inet.h"
#include "bdecode.h"
#include <cstring>
#include <exception>
#include <iostream>
#include <mutex>
#include <sstream>

Peer::Peer() {}

Peer::Peer(const std::string &raw_code) { Parse(raw_code); }

void Peer::Parse(const std::string &raw_code) {
  std::stringstream ss(raw_code);
  BDecoder decoder;
  decoder.Decode(ss);
  try {
    auto root = decoder.Cast<BDecoder::DictValue>();
    auto it = root.find("peers");
    if (it != root.end()) {
      auto peers = BDecoder::Cast<BDecoder::StringValue>(it->second);
      int n_peers = peers.size() / PEER_SIZE;
      const char *data = peers.data();
      for (size_t i = 0; i < peers.size(); i += PEER_SIZE) {
        in_addr addr;
        u_int16_t port;
        memcpy(&addr, data + i, PEER_IP_SIZE);
        memcpy(&port, data + i + PEER_IP_SIZE, PEER_PORT_SIZE);
        port = ntohs(port);

        std::lock_guard<std::mutex> lock(m_mutex);
        auto addr_char = inet_ntoa(addr);
        std::cout << addr_char << ':' << port << std::endl;
        m_peers.push_back({std::string(addr_char), port});
      }
    }
  } catch (std::exception &e) {
    if (decoder.GetRoot()) {
      std::cout << decoder.GetRoot()->raw_value;
      return;
    }
    std::cout << "value is nullptr" << std::endl;
  }
}