#include "ServicePool.h"

ServicePool &ServicePool::GetInstace()
{
    static ServicePool s;
    return s;
}

ServicePool::ServicePool(size_t size) : m_size(size), m_iocs(size), m_works(size), m_next(0)
{
    for (size_t i = 0; i < size; i++)
    {
        m_works[i] = std::make_unique<io_context::work>(m_iocs[i]);
    }
    for (size_t i = 0; i < size; i++)
    {
        m_threads.emplace_back([this, i]()
                               { 
                                m_iocs[i].run(); });
    }
}

io_context &ServicePool::GetService()
{
    auto& ioc = m_iocs[(m_next++) % m_size];
    return ioc;
}

void ServicePool::Stop(){
    for (auto &work : m_works)
    {
        work.reset();
    }
    for (auto &thread : m_threads)
    {
        thread.join();
    }
}

ServicePool::~ServicePool(){
}