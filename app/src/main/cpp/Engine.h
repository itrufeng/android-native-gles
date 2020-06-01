//
// Created by Jian Zhang on 5/26/20.
//

#ifndef ANDROID_NATIVE_GLES_ENGINE_H
#define ANDROID_NATIVE_GLES_ENGINE_H

#include <EGL/egl.h>

namespace nativelib {

    class EGLHelper;

    class Engine {
    public:
        Engine();
        struct android_app* app;
        EGLDisplay display;
        EGLSurface surface;
        EGLContext context;

        int engine_init_display();
    private:
        EGLHelper* helper;
    };
}

#endif //ANDROID_NATIVE_GLES_ENGINE_H
