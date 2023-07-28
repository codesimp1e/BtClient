#pragma once

#include "torrent.h"
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <queue>

// 16KB
#define BLOCK_SIZE 16384

class PeerWorkers {
public:
  class WorkerInfo {
  public:
    using ptr = std::shared_ptr<WorkerInfo>;
    WorkerInfo(uint32_t index, uint32_t offset, uint32_t length);
    uint32_t index;
    uint32_t offset;
    uint32_t length;
  };

  using WorksPtr = std::shared_ptr<std::queue<WorkerInfo::ptr>>;

  PeerWorkers(const PeerWorkers &) = delete;
  PeerWorkers &operator=(const PeerWorkers &) = delete;

  static PeerWorkers &GetInstance();
  void Push(Torrent *torrent, std::shared_ptr<WorkerInfo>);
  std::shared_ptr<WorkerInfo> Pop(Torrent *torrent);
  void SetTorrent(Torrent *torrent);

private:
  PeerWorkers(size_t max_size = 9999);
  std::mutex m_mutex_works;
  std::mutex m_mutex_order;
  std::map<Torrent *, std::shared_ptr<std::mutex>> m_mutexes;
  std::map<Torrent *, WorksPtr> m_works;
  std::queue<Torrent *> m_order;
  std::condition_variable m_cv;
  void Start();
  void HandleTorrent(Torrent *torrent, WorksPtr works,
                     std::shared_ptr<std::mutex> mutex);
};