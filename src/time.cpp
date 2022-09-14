#include "zinc/time.h"
#include <limits>

namespace zinc {
duration duration::SECOND = duration::from_seconds(1);
duration duration::MILLISECOND = duration::from_milliseconds(1);
duration duration::MICROSECOND = duration::from_microseconds(1);
duration duration::NANOSECOND = duration::from_nanoseconds(1);
duration duration::ZERO = duration::from_seconds(0);
duration duration::MAX =
    duration::from_seconds(std::numeric_limits<u64>().max());
} // namespace zinc