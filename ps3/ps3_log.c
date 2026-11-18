#include "ps3_platform.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>

#ifndef PKG_BUILD_STAMP
#define PKG_BUILD_STAMP "0.1-dev"
#endif

static FILE *s_log_fp = NULL;

void ps3_log_init(void)
{
    if (s_log_fp) return;
    s_log_fp = fopen(PS3_LOG_PATH, "a");
    if (s_log_fp) {
        fprintf(s_log_fp, "=== LOG ABIERTO ===\n"); fflush(s_log_fp); fprintf(s_log_fp, "=== LOG ABIERTO ===\n"); fflush(s_log_fp); ps3_log("=== INICIO DE SESION [PKG: PCFX-PS3_%s] ===\n", PKG_BUILD_STAMP);
    }
}

void ps3_log_close(void)
{
    if (s_log_fp) {
        ps3_log("=== FIN DE SESION [PKG: PCFX-PS3_%s] ===\n\n", PKG_BUILD_STAMP);
        fflush(s_log_fp);
        fclose(s_log_fp);
        s_log_fp = NULL;
    }
}

void ps3_log(const char *fmt, ...)
{
    if (!s_log_fp) {
        s_log_fp = fopen(PS3_LOG_PATH, "a");
        if (!s_log_fp) return;
    }

    struct timeval tv;
    gettimeofday(&tv, NULL);
    time_t now = tv.tv_sec;
    struct tm *t = localtime(&now);

    if (t) {
        fprintf(s_log_fp, "[%04d-%02d-%02d %02d:%02d:%02d.%03d] [%s] ",
                t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                t->tm_hour, t->tm_min, t->tm_sec, (int)(tv.tv_usec / 1000),
                PKG_BUILD_STAMP);
    }

    va_list ap;
    va_start(ap, fmt);
    vfprintf(s_log_fp, fmt, ap);
    va_end(ap);
    fflush(s_log_fp);
}
