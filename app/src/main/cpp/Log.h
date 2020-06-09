//
// Created by Jian Zhang on 5/26/20.
//

#ifndef ANDROID_NATIVE_GLES_LOG_H
#define ANDROID_NATIVE_GLES_LOG_H

#include <string>

using namespace std;

namespace nativelib {
    class Log {
    public:
        Log(const string tag);
        void v(const char *fmt, ...);
        void d(const char *fmt, ...);
        void i(const char *fmt, ...);
        void w(const char *fmt, ...);
        void e(const char *fmt, ...);
        void f(const char *fmt, ...);

    private:
        string tag;
    };
}


#endif //ANDROID_NATIVE_GLES_LOG_H
