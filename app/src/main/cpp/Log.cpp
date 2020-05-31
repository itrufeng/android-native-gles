//
// Created by Jian Zhang on 5/26/20.
//

#include "Log.h"
#include <android/log.h>

Log::Log(const string tag) {
    this->tag = tag;
}

void Log::v(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    __android_log_vprint(ANDROID_LOG_VERBOSE, this->tag.c_str(), fmt, ap);
    va_end(ap);
}

void Log::d(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    __android_log_vprint(ANDROID_LOG_DEBUG, this->tag.c_str(), fmt, ap);
    va_end(ap);
}

void Log::i(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    __android_log_vprint(ANDROID_LOG_INFO, this->tag.c_str(), fmt, ap);
    va_end(ap);
}

void Log::w(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    __android_log_vprint(ANDROID_LOG_WARN, this->tag.c_str(), fmt, ap);
    va_end(ap);
}

void Log::e(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    __android_log_vprint(ANDROID_LOG_ERROR, this->tag.c_str(), fmt, ap);
    va_end(ap);
}

void Log::f(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    __android_log_vprint(ANDROID_LOG_FATAL, this->tag.c_str(), fmt, ap);
    va_end(ap);
}
