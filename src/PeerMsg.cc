#include "PeerMsg.h"
#include <cstdint>
#include <cstring>
#include <netinet/in.h>

PeerMsg::PeerMsg(uint32_t len) : m_data(new uint8_t[len]), m_len(len) {}
PeerMsg::~PeerMsg() { delete[] m_data; };
void PeerMsg::Marshal() {
  uint8_t type;
  memcpy(&type, m_data, 1);
  m_type = Type(type);
}

std::string PeerMsg::TypeString() { return TypeString(m_type); }

std::string PeerMsg::TypeString(Type type) {
  switch (type) {
  case Type::choke:
    return "choke";
  case Type::unchoke:
    return "unchoke";
  case Type::interested:
    return "interested";
  case Type::not_interested:
    return "not_interested";
  case Type::have:
    return "have";
  case Type::bitfield:
    return "bitfield";
  case Type::cancel:
    return "cancel";
  case Type::piece:
    return "piece";
  case Type::request:
    return "request";
  }
}
