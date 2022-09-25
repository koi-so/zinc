#include "zinc/allocator/pool.h"

#include "zinc/debug.h"

namespace zinc {
pool::pool(usize granularity, usize size)
    : m_granularity(granularity), m_size(size), m_used(0), m_overflow(0) {
  if (m_size > 0) {
    m_storage.reset(new char[m_size * granularity]);
    m_slots.reset(new vptr[m_size]);

    for (usize i = 0; i < m_size; ++i)
      m_slots[i] = reinterpret_cast<vptr>(m_storage.get() + i * granularity);
  }
}

pool::~pool() {
  ZINC_ASSERTF(m_used == 0 && m_overflow == 0,
               "can't destroy a pool with outstanding allocations");
}

auto pool::allocate() -> vptr {
  if (m_used < m_size) {
    return m_slots[m_used++];
  } else {
    ++m_overflow;
    return reinterpret_cast<vptr>(new char[m_granularity]);
  }
}

void pool::deallocate(vptr block) {
  ZINC_ASSERTF(block, "null pointer argument");
  if (is_from_pool(block)) {
    ZINC_ASSERTF(m_used > 0, "internal error");
    m_slots[--m_used] = block;
  } else {
    ZINC_ASSERTF(m_overflow > 0, "internal error");
    delete[] reinterpret_cast<char *>(block);
    --m_overflow;
  }
}

} // namespace zinc