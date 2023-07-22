#pragma once

#include "PeerConnect.h"
#include "peer.h"
#include <boost/asio/ssl.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/beast.hpp>
#include <boost/url/parse.hpp>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace ssl = boost::asio::ssl;

class TrackerConnect;

class Torrent {
  friend class TrackerConnect;

public:
  Torrent(const char *file, int port);
  ~Torrent();
  class Info {
  public:
    long long length = 0;
    std::string m_name = "";
    char *m_pieces = nullptr;
    long long m_pieces_len = 0;
    std::vector<std::map<std::string, long long>> files;
  };

  bool Parse();
  std::string ToString();
  void FindPeers();
  std::string GetQuery();
  void DelConnect(const std::string &);

  void AddPeerConnect(const std::string& key,std::shared_ptr<PeerConnect> peer_connect);
  void DelPeerConnect(const std::string& key);

private:
  std::string Query();
  std::string m_file;
  std::string m_announce;
  std::string m_comment;
  std::string m_info_hash;
  std::string m_info_url;
  std::string m_peer_id;
  std::string m_query;

  std::map<std::string, std::shared_ptr<TrackerConnect>> m_connects;
  std::map<std::string, std::shared_ptr<PeerConnect>> m_peer_connects;

  int m_port;
  Info m_info;
  Peer m_peer;
  std::vector<std::vector<std::string>> m_announce_list;

  ssl::context m_ctx;
};

class TrackerEvent {
public:
  friend class Http;
  TrackerEvent(const char *, size_t, std::shared_ptr<Torrent>);
  using ptr = std::shared_ptr<TrackerEvent>;
  ~TrackerEvent();

private:
  char *m_req;
  std::shared_ptr<Torrent> m_torrent;
};