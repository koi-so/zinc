#include "zinc/debug.h"

#if ZINC_PLATFORM_ANDROID
#include <android/log.h>
#elif ZINC_PLATFORM_WINDOWS
extern "C" __declspec(dllimport) void __stdcall OutputDebugStringA(
    const char *_str);
#else
#endif

namespace zinc {
void DefaultAssertHandler(const char *text, const char *sourcefile, u32 line) {
  char output_text[2048];
  if (sourcefile) {
    snprintf(output_text, sizeof(output_text), "%s(%d): ASSERT FAILURE - ",
             sourcefile, line);
  } else {
    strcpy_s(output_text, sizeof(output_text), "ASSERT FAILURE - ");
  }
  strcat_s(output_text, sizeof(output_text), text);

  // print
#if ZINC_PLATFORM_ANDROID
  __android_log_write(ANDROID_LOG_DEBUG, "", output_text);
#elif ZINC_PLATFORM_WINDOWS
  strcat_s(output_text, sizeof(output_text), "\n");
  OutputDebugStringA(output_text);
#else
  fputs(output_text, stderr);
  fflush(stderr);
#endif
}

static AssertCallback *s_assert_handler = DefaultAssertHandler;

void SetAssertCallback(AssertCallback *callback) {
  s_assert_handler = callback ? callback : DefaultAssertHandler;
}

void DebugMessage(const char *sourcefile, u32 line, const char *fmt...) {
  char text[2048];

  va_list args;
  va_start(args, fmt);
  vsnprintf(text, sizeof(text), fmt, args);
  va_end(args);

  s_assert_handler(text, sourcefile, line);
}
} // namespace zinc