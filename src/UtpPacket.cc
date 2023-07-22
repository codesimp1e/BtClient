#include "UtpPacket.h"
#include <arpa/inet.h>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <netinet/in.h>
#include <random>
#include <sstream>

UtpHeader::UtpHeader() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<uint16_t> dis;
  m_id = dis(gen);
}

void UtpHeader::SetTime(uint32_t time) { m_time = time; }
void UtpHeader::SetWndSize(uint32_t wnd_size) { m_wnd_size = wnd_size; }

const char *UtpHeader::GetData() {
  uint8_t begin = (m_type << 4) | m_version;
  memcpy(m_data, &begin, sizeof(begin));

  memcpy(m_data + 1, &m_extension, sizeof(m_extension));

  uint16_t id_net = htons(m_id);
  memcpy(m_data + 2, &id_net, sizeof(id_net));

  uint32_t time_net = htonl(m_time);
  memcpy(m_data + 4, &time_net, sizeof(time_net));

  uint32_t time_diff_net = htonl(m_time_diff);
  memcpy(m_data + 8, &time_net, sizeof(time_diff_net));

  uint32_t wnd_size_net = htonl(m_wnd_size);
  memcpy(m_data + 12, &time_net, sizeof(wnd_size_net));

  uint16_t seq_net = htons(m_seq_nr);
  memcpy(m_data + 16, &time_net, sizeof(seq_net));

  uint16_t ack_net = htons(m_ack_nr);
  memcpy(m_data + 18, &time_net, sizeof(ack_net));

  return m_data;
}

void UtpHeader::SetData(const uint8_t *data) {
  uint8_t begin;
  memcpy(&begin, data, sizeof(begin));
  m_type = begin >> 4;
  m_version = begin & 0x0F;
  memcpy(&m_extension, data + 1, sizeof(m_extension));

  memcpy(&m_id, data + 2, sizeof(m_id));
  m_id = ntohs(m_id);

  memcpy(&m_time, data + 4, sizeof(m_time));
  m_time = ntohl(m_time);

  memcpy(&m_time_diff, data + 8, sizeof(m_time_diff));
  m_time_diff = ntohl(m_time_diff);

  memcpy(&m_wnd_size, data + 12, sizeof(m_wnd_size));
  m_wnd_size = ntohl(m_wnd_size);

  memcpy(&m_seq_nr, data + 16, sizeof(m_seq_nr));
  m_seq_nr = ntohs(m_seq_nr);

  memcpy(&m_ack_nr, data + 18, sizeof(m_ack_nr));
  m_ack_nr = ntohs(m_ack_nr);
}

std::string UtpHeader::ToString() {
  std::stringstream ss;
  ss << "type:" << (int)m_type << std::endl
     << "version:" << (int)m_version << std::endl
     << "extension:" << (int)m_extension << std::endl
     << "id:" << m_id << std::endl
     << "time" << m_time << std::endl
     << "time_diff:" << m_time_diff << std::endl
     << "wnd_size:" << m_wnd_size << std::endl
     << "seq_nr" << m_seq_nr << std::endl
     << "ack_nr" << m_ack_nr << std::endl;
  ;
  return ss.str();
}