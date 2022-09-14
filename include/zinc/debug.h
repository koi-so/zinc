#pragma once

#include "base.h"

namespace zinc {
using AssertCallback = void(char const *, char const *, u32);

ZINC_API void DebugMessage(char const *sourcefile, u32 line,
                           char const *fmt...);
ZINC_API void SetAssertCallback(AssertCallback *callback);
} // namespace zinc

#if ZINC_COMPILER_MSVC && !ZINC_COMPILER_CLANG_CL
#define zinc_hwbreak() __debugbreak()
#elif ZINC_COMPILER_CLANG
#if (__has_builtin(__builtin_debugtrap))
#define zinc_hwbreak() __builtin_debugtrap()
#else
#define zinc_hwbreak()                                                         \
  __builtin_trap() // this cannot be used in constexpr functions
#endif
#elif ZINC_COMPILER_GCC
#define zinc_hwbreak() __builtin_trap()
#endif

#if ZINC_CONFIG_ENABLE_ASSERT
#define ZINC_ASSERT(_e)                                                        \
  do {                                                                         \
    if (!(_e)) {                                                               \
      zinc::DebugMessage(__FILE__, __LINE__, #_e);                             \
      zinc_hwbreak();                                                          \
    }                                                                          \
  } while (0)
#define ZINC_ASSERTF(_e, ...)                                                  \
  do {                                                                         \
    if (!(_e)) {                                                               \
      zinc::DebugMessage(__FILE__, __LINE__, __VA_ARGS__);                     \
      zinc_hwbreak();                                                          \
    }                                                                          \
  } while (0)
#else
#define ZINC_ASSERT(_e)
#define ZINC_ASSERTF(_e, ...)
#endif

#define zinc_unreachable() ZINC_ASSERT(0, "unreachable code");