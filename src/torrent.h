#pragma once

#include "bdecode.h"
#include <string>
#include <vector>
#include <map>
#include <iostream>

class Torrent
{
public:
  Torrent(const char *file);
  ~Torrent();
  class Info
  {
  public:
    long long length = 0;
    std::string m_name = "";
    char *m_pieces = nullptr;
    long long m_pieces_len = 0;
    std::vector<std::map<std::string, long long>> files;
  };

  std::string ToString();
  int FindPeers();

private:
  std::string m_file;
  std::string m_announce;
  std::string m_comment;
  std::string m_info_hash;
  Info m_info;
  std::vector<std::vector<std::string>> m_announce_list;
};
