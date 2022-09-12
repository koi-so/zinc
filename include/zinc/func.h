#pragma once

#include "base.h"

namespace zinc {
template <typename T> class func;

template <typename R, typename... Args> class func<R(Args...)> {
  // function pointer types for the type-erasure behaviors
  // all these char* parameters are actually casted from some functor type
  typedef R (*invoke_fn_t)(char *, Args &&...);
  typedef void (*construct_fn_t)(char *, char *);
  typedef void (*destroy_fn_t)(char *);

  // type-aware generic functions for invoking
  // the specialization of these functions won't be capable with
  //   the above function pointer types, so we need some cast
  template <typename Functor>
  static auto invoke_fn(Functor *fn, Args &&...args) -> R {
    return (*fn)(forward<Args>(args)...);
  }

  template <typename Functor>
  static void construct_fn(Functor *construct_dst, Functor *construct_src) {
    // the functor type must be copy-constructible
    new (construct_dst) Functor(*construct_src);
  }

  template <typename Functor> static void destroy_fn(Functor *f) {
    f->~Functor();
  }

  // these pointers are storing behaviors
  invoke_fn_t invoke_f;

public:
  func() : invoke_f(nullptr) {}

  // construct from any functor type
  template <typename Functor>
  func(Functor f)
      // specialize functions and erase their type info by casting
      : invoke_f(reinterpret_cast<invoke_fn_t>(invoke_fn<Functor>)) {
    // copy the functor to internal storage
    this->construct_f(this->data_ptr.get(), reinterpret_cast<char *>(&f));
  }

  // copy constructor
  func(func const &rhs) : invoke_f(rhs.invoke_f) {
    if (this->invoke_f) {
      // when the source is not a null function, copy its internal functor
      this->data_ptr.reset(new char[this->data_size]);
      this->construct_f(this->data_ptr.get(), rhs.data_ptr.get());
    }
  }

  ~func() = default;

  // other constructors, from nullptr, from function pointers

  auto operator()(Args &&...args) -> R {
    return this->invoke_f(this->data_ptr.get(), forward<Args>(args)...);
  }
};
} // namespace zinc