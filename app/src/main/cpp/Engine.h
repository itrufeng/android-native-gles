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

    class Log;

    class Engine {
    public:
        Engine(struct android_app *app);

        int InitDisplay();

        void DestroyDisplay();

        void Prepare();

        void DrawFrame();

        bool IsAppWindowCreated();

    private:
        struct android_app *app;
        EGLDisplay display;
        EGLSurface surface;
        EGLContext context;
        Log *logger;

        void ReadShaderSource(string path, char *out_source);

        int32_t SelectConfigForPixelFormat(EGLDisplay dpy, EGLint const *attrs,
                                           int32_t format, EGLConfig *outConfig);

        int32_t SelectConfigForNativeWindow(EGLDisplay dpy, EGLint const *attrs,
                                            EGLNativeWindowType window, EGLConfig *outConfig);

        void CheckMaxVertexAttribs();

        void CreateShader(const GLchar *const *source, GLenum type, GLuint *shader);

        void CreateProgram(GLuint vertexShader, GLuint fragmentShader, GLuint *program);
    };
}

#endif //ANDROID_NATIVE_GLES_ENGINE_H
