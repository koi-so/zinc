#pragma once

#include "base.h"

namespace zinc {
template <typename> struct func;
template <typename Return, typename... Args> struct func<Return(Args...)> {
private:
  struct base_wrapper {
    Return (*thunk)(base_wrapper *wrapper, Args... args) = nullptr;
  };

  template <typename TCallable> struct _wrapper : base_wrapper {
    std::decay_t<TCallable> callable;

    static auto thunk(base_wrapper *wrapper, Args... args) -> Return {
      return static_cast<_wrapper *>(wrapper)->callable(
          std::forward<Args>(args)...);
    }
    template <typename I>
    inline _wrapper(I &&callable)
        : base_wrapper{&thunk}, callable{std::forward<I>(callable)} {}
  };

  base_wrapper *wrapper = nullptr;

public:
  inline func() = default;
  inline func(func &&other) : wrapper{other.wrapper} {
    other.wrapper = nullptr;
  }
  template <typename TCallable,
            typename = std::void_t<
                decltype(std::declval<TCallable>()(std::declval<Args>()...))>>
  inline func(TCallable &&callable)
      : wrapper{new _wrapper<TCallable>{std::forward<TCallable>(callable)}} {}
  inline ~func() {
    if (this->wrapper) {
      delete this->wrapper;
    }
  }
  inline void operator=(func &&other) {
    this->wrapper = other.wrapper;
    other.wrapper = nullptr;
  }
  [[nodiscard]] inline auto is_valid() const -> bool {
    return this->wrapper != nullptr;
  }
  inline explicit operator bool() const { return this->wrapper != nullptr; }
  inline auto operator()(Args... args) const -> Return {
    return this->wrapper->thunk(this->wrapper, std::forward<Args>(args)...);
  }
};
} // namespace zinc