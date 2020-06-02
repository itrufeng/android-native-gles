//
// Created by Jian Zhang on 5/26/20.
//

#ifndef ANDROID_NATIVE_GLES_ENGINE_H
#define ANDROID_NATIVE_GLES_ENGINE_H

#include <EGL/egl.h>
#include <android_native_app_glue.h>

namespace nativelib {

    class EGLHelper;

    class Engine {
    public:
        Engine();
        struct android_app* app;
        EGLDisplay display;
        EGLSurface surface;
        EGLContext context;

        int init_display();
        int destroy_display();
        void prepare();
        void draw_frame();
        bool isAppWindowCreated();
    private:
        EGLHelper* helper;
        void read_shader_source(char* path, char* out_source);
    };
}

#endif //ANDROID_NATIVE_GLES_ENGINE_H
