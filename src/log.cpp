#include <common.hpp>
#include "log.hpp"

EM_JS(void, console_log, (const char* str), {
  console.log(UTF8ToString(str));
} );

void outf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char buf[LOG_MAXLEN];
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    console_log(buf);
}
