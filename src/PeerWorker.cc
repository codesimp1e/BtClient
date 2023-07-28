#include "PeerWorker.h"
#include "torrent.h"
#include <cstddef>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

PeerWorkers &PeerWorkers::GetInstance() {
  static PeerWorkers pw;
  return pw;
}

void PeerWorkers::Push(Torrent *torrent, std::shared_ptr<WorkerInfo> work) {
  std::lock_guard<std::mutex> lock(m_mutex_works);
  auto it = m_works.find(torrent);
  if (it != m_works.end()) {
    it->second->push(work);
  }
}

std::shared_ptr<PeerWorkers::WorkerInfo> PeerWorkers::Pop(Torrent *torrent) {
  std::lock_guard<std::mutex> lock(m_mutex_works);
  auto it = m_works.find(torrent);
  if (it == m_works.end() || it->second->empty()) {
    return nullptr;
  }
  auto work = it->second->front();
  it->second->pop();
  return work;
}

PeerWorkers::PeerWorkers(size_t max_size) {}

void PeerWorkers::SetTorrent(Torrent *torrent) {
  std::lock_guard<std::mutex> lock(m_mutex_order);
  m_order.push(torrent);
}

void PeerWorkers::Start() {
  std::vector<std::thread> threads;
  auto torrent = m_order.front();
  std::lock_guard<std::mutex> lock(m_mutex_works);
  auto works = std::make_shared<std::queue<WorkerInfo::ptr>>();
  auto mutex = std::make_shared<std::mutex>();
  m_works[torrent] = works;
  m_mutexes[torrent] = mutex;
  threads.emplace_back([this, torrent, works, mutex]() {
    HandleTorrent(torrent, works, mutex);
  });
}

void PeerWorkers::HandleTorrent(Torrent *torrent, WorksPtr works,
                                std::shared_ptr<std::mutex> mutex) {
  size_t piece_len = torrent->m_info.m_pieces_len;
  size_t n_pieces = torrent->m_info.length / piece_len;
  size_t pieces_left = torrent->m_info.length % torrent->m_info.m_pieces_len;
  if (pieces_left) {
    n_pieces++;
  } else {
    pieces_left = piece_len;
  }
  for (size_t idx = 0; idx < n_pieces; idx++) {
    if (idx == n_pieces - 1) {
      piece_len = pieces_left;
    }
    size_t block_len = BLOCK_SIZE;
    size_t n_blocks = piece_len / block_len;
    size_t block_left = piece_len % block_len;
    if (block_left) {
      n_blocks++;
    } else {
      block_left = block_len;
    }

    for (size_t idx_block = 0; idx_block < n_blocks; idx_block += BLOCK_SIZE) {
      if (idx_block == n_blocks - 1) {
        block_len = block_left;
      }
      std::lock_guard<std::mutex> lock(*mutex);
      works->emplace(new WorkerInfo(idx, idx_block * BLOCK_SIZE, block_len));
    }
  }
}
