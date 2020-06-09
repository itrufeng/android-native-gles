//
// Created by Jian Zhang on 5/26/20.
//

#ifndef ANDROID_NATIVE_GLES_ENGINE_H
#define ANDROID_NATIVE_GLES_ENGINE_H

#include <string>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <android_native_app_glue.h>

using namespace std;

namespace nativelib {

    class EGLHelper;
    class Log;

    class Engine {
    public:
        Engine();
        struct android_app* app;
        EGLDisplay display;
        EGLSurface surface;
        EGLContext context;

        int init_display();
        void destroy_display();
        void prepare();
        void draw_frame();
        bool isAppWindowCreated();
    private:
        EGLHelper* helper;
        Log *logger;
        void read_shader_source(string path, char* out_source);
        void CreateShader(const GLchar *const*source, GLenum type, GLuint* shader);
        void CreateProgram(GLuint vertexShader, GLuint fragmentShader, GLuint *program);
    };
}

#endif //ANDROID_NATIVE_GLES_ENGINE_H
