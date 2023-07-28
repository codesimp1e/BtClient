#pragma once

#include <cstdint>
#include <string>

class PeerConnectTcp;

class PeerMsg {
public:
  friend class PeerConnectTcp;
  enum class Type {
    choke = 0,
    unchoke,
    interested,
    not_interested,
    have,
    bitfield,
    request,
    piece,
    cancel,
  };
  PeerMsg(uint32_t len);
  ~PeerMsg();
  void Marshal();
  static std::string TypeString(Type);
  std::string TypeString();

private:
  uint8_t *m_data;
  uint32_t m_len;
  Type m_type;
};