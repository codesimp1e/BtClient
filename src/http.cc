#include "http.h"
#include <cstring>
#include "torrent.h"

void Http::Handle(const std::string &req, std::shared_ptr<Torrent> torrent)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_send_queue.push(TrackerEvent::ptr(new TrackerEvent(req.c_str(), req.size(), torrent)));
    if (m_send_queue.size() == 1)
    {
        m_consume.notify_one();
    }
}

Http::Http(size_t size) : m_service_pool(ServicePool::GetInstace()), m_stop(false)
{
    m_work_thread = std::thread([this]()
                                { Start(); });
}

Http::~Http()
{
    m_stop = true;
    m_consume.notify_one();
    m_work_thread.join();
}

Http &Http::GetInstance()
{
    static Http http;
    return http;
}

void Http::HandleTrackerEvent()
{
    auto event = m_send_queue.front();
    event->m_torrent->FindPeers();
    m_send_queue.pop();
}

void Http::Start()
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (m_send_queue.empty() && !m_stop)
        {
            m_consume.wait(lock);
        }
        if (m_stop)
        {
            while (!m_send_queue.empty())
            {
                HandleTrackerEvent();
            }
            break;
        }
        HandleTrackerEvent();
    }
}