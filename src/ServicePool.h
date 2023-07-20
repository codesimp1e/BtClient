#pragma once
#include <boost/asio.hpp>
#include <thread>
#include <vector>
#include <memory>

namespace asio = boost::asio;
using io_context = asio::io_context;

class ServicePool
{
public:
    ~ServicePool();
    ServicePool(const ServicePool &) = delete;
    ServicePool &operator=(const ServicePool &) = delete;
    static ServicePool &GetInstace();
    io_context &GetService();
    void Stop();

private:
    ServicePool(size_t size = std::thread::hardware_concurrency());
    std::vector<std::thread> m_threads;
    std::vector<io_context> m_iocs;
    std::vector<std::unique_ptr<io_context::work>> m_works;
    size_t m_next;
    size_t m_size;
};