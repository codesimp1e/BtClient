#pragma once
#include "TrackerConnect.h"
#include <mutex>
#include <string>
#include <vector>

#define PEER_SIZE 6
#define PEER_IP_SIZE 4
#define PEER_PORT_SIZE 2

class Peer
{
    friend class TrackerConnect;
    struct peer_t
    {
        std::string ip;
        unsigned short port;
        peer_t(const std::string &ip, unsigned short port) : ip(ip), port(port)
        {
        }
    };

public:
    Peer();
    Peer(const std::string &);
    bool Parse(const std::string &raw_code);

private:
    std::vector<peer_t> m_peers;
    std::mutex m_mutex;
};