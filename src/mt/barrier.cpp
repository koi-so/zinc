#include "zinc/mt/barrier.h"

namespace zinc {
barrier::barrier(usize thread_count)
    : m_thread_count(thread_count), m_waiting_count(0) {}

void barrier::wait() {
  std::unique_lock<std::mutex> lock(m_mutex);
  ++m_waiting_count;
  if (m_waiting_count == m_thread_count) {
    m_waiting_count = 0;
    m_condition.notify_all();
  } else {
    m_condition.wait(lock, [this] { return m_waiting_count == 0; });
  }
}
} // namespace zinc