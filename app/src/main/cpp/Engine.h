//
// Created by Jian Zhang on 5/26/20.
//

#ifndef ANDROID_NATIVE_GLES_ENGINE_H
#define ANDROID_NATIVE_GLES_ENGINE_H

#include <EGL/egl.h>

namespace nativelib {
    class Engine {
    public:
        struct android_app* app;
        EGLDisplay display;
        EGLSurface surface;
        EGLContext context;
    };
}

#endif //ANDROID_NATIVE_GLES_ENGINE_H
