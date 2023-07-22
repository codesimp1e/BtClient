#include <cstdint>
#include <cstring>
#include <iostream>

int main() {
  uint8_t type = 4, version = 1;
  uint8_t begin = (type << 4) | version;
  std::cout << (int)begin << std::endl;
  std::cout << std::hex << (int)begin << std::endl;
  type = begin >> 4;
  version = begin & 0x0f;
  std::cout << (int)type << ',' << (int)version << std::endl;
}