#include <cstdint>
#include <string>

#define UTP_HEADER_LENGTH 20

class UtpHeader {

public:
  enum { ST_DATA = 0, ST_FIN = 1, ST_STATE = 2, ST_RESET = 3, ST_SYN = 4 };
  UtpHeader();

  void SetTime(uint32_t time);
  void SetWndSize(uint32_t wnd_size);
  const char *GetData();
  void SetData(const uint8_t *data);
  std::string ToString();

private:
  uint8_t m_type = ST_SYN;
  uint8_t m_version = 1;
  uint8_t m_extension = 0;
  uint16_t m_id;
  uint32_t m_time;
  uint32_t m_time_diff = 0;
  uint32_t m_wnd_size = 0;
  uint16_t m_seq_nr = 1;
  uint16_t m_ack_nr = 0;
  char m_data[UTP_HEADER_LENGTH];
};

class UtpPayload {};

class UtpPacket {
public:
  UtpHeader header;
  UtpPayload payload;
};