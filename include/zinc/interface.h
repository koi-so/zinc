#pragma once

namespace zinc {
class interface {
public:
  virtual ~interface() = default;

  template <typename T> auto as() & -> T & { return static_cast<T &>(*this); }

  template <typename T> auto as() const & -> T const & {
    return static_cast<T &>(*this);
  }
};
} // namespace zinc