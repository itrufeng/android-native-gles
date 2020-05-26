//
// Created by Jian Zhang on 5/26/20.
//

#include <android/log.h>

void logV(const char *tag, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    __android_log_vprint(ANDROID_LOG_VERBOSE, tag, fmt, ap);
    va_end(ap);
}

void logD(const char *tag, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    __android_log_vprint(ANDROID_LOG_DEBUG, tag, fmt, ap);
    va_end(ap);
}

void logI(const char *tag, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    __android_log_vprint(ANDROID_LOG_INFO, tag, fmt, ap);
    va_end(ap);
}

void logW(const char *tag, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    __android_log_vprint(ANDROID_LOG_WARN, tag, fmt, ap);
    va_end(ap);
}

void logE(const char *tag, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    __android_log_vprint(ANDROID_LOG_ERROR, tag, fmt, ap);
    va_end(ap);
}

void logF(const char *tag, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    __android_log_vprint(ANDROID_LOG_FATAL, tag, fmt, ap);
    va_end(ap);
}