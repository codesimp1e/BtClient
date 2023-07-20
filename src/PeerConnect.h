#pragma once

#include <memory>
#include <boost/asio.hpp>

namespace asio = boost::asio;
using tcp = boost::asio::ip::tcp;

class PeerConnect : public std::enable_shared_from_this<PeerConnect> {
public:
  PeerConnect();

private:
  tcp::socket m_socket;
};