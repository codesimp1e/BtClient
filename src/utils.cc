#include "utils.h"
#include <iomanip>
#include <sstream>
std::string UrlEncode(const std::string &input) {
  std::stringstream ss;
  ss.fill('0');
  ss << std::hex;

  for (const auto &c : input) {
    if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
      ss << c;
      continue;
    }
    // ss << std::uppercase;
    ss << '%' << std::setw(2) << int((unsigned char)c);
    // ss << std::nouppercase;
  }
  return ss.str();
}

std::string to_hex(const uint8_t *data, int len) {
  std::stringstream ss;
  ss << std::uppercase << std::hex << std::setfill('0');
  for (int i = 0; i < len; i++) {
    ss << std::setw(2) << static_cast<unsigned>(data[i]);
  }
  return ss.str();
}