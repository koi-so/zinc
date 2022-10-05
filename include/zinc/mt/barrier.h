#pragma once

#include "../base.h"

#include <condition_variable>
#include <mutex>
#include <thread>


namespace zinc {
struct barrier : public zinc::non_copyable {
public:
  barrier(usize thread_count);

  void wait();

private:
  usize m_thread_count;
  usize m_waiting_count;
  std::mutex m_mutex;
  std::condition_variable m_condition;
};
} // namespace zinc