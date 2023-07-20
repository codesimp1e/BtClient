#pragma once
#include <mutex>
#include <string>
#include <vector>

#define PEER_SIZE 6
#define PEER_IP_SIZE 4
#define PEER_PORT_SIZE 2

class Peer
{
    struct peer_t
    {
        std::string ip;
        short port;
        peer_t(const std::string &ip, unsigned short port) : ip(ip), port(port)
        {
        }
    };

public:
    Peer();
    Peer(const std::string &);
    void Parse(const std::string &raw_code);

private:
    std::vector<peer_t> m_peers;
    std::mutex m_mutex;
};