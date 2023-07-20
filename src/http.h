#include <thread>
#include <queue>
#include <memory>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <map>
#include "ServicePool.h"

class Torrent;
class TrackerEvent;

class Http
{
public:
    using CallBack = std::function<void(const char *, size_t)>;
    
    Http(const Http &) = delete;
    Http &operator=(const Http &) = delete;
    ~Http();
    static Http &GetInstance();
    void Handle(const std::string &, std::shared_ptr<Torrent> torrent);

private:
    void HandleTrackerEvent();
    void Start();
    std::mutex m_mutex;
    Http(size_t size = std::thread::hardware_concurrency());
    std::queue<std::shared_ptr<TrackerEvent>> m_send_queue;
    std::condition_variable m_consume;
    std::map<short, CallBack> m_cbs;
    ServicePool & m_service_pool;
    std::thread m_work_thread;
    bool m_stop;
};

