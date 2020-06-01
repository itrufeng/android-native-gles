//
// Created by Jian Zhang on 6/1/20.
//

#include "EGLHelper.h"
#include <GLES3/gl3.h>
#include <android/native_window.h>
#include <android_native_app_glue.h>
#include "Log.h"
#include "Engine.h"

int nativelib::EGLHelper::engine_init_display(Engine *engine) {
    // initialize OpenGL and EGL
    EGLint majorVersion;
    EGLint minorVersion;
    EGLBoolean eglInitializeState;

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitializeState = eglInitialize(display, &majorVersion, &minorVersion);
    engine->display = display;

    auto logger = Log("NativeGL:main");
    logger.i("EGL initial result: %d & version: %d.%d", eglInitializeState, majorVersion, minorVersion);

    EGLConfig config;
    const EGLint attrs[] = {
            EGL_DEPTH_SIZE, 16,
            EGL_NONE
    };
    selectConfigForNativeWindow(display, attrs, engine->app->window, &config);

    EGLSurface surface;
    surface = eglCreateWindowSurface(display, config, engine->app->window, NULL);
    engine->surface = surface;

    EGLContext context;
    const EGLint GLES3[] = {
            EGL_CONTEXT_CLIENT_VERSION, 3,
            EGL_NONE
    };
    context = eglCreateContext(display, config, NULL, GLES3);
    engine->context = context;

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        logger.e("Unable to eglMakeCurrent");
        return -1;
    }

    checkMaxVertexAttribs();

    return 0;
}

int32_t nativelib::EGLHelper::selectConfigForPixelFormat(EGLDisplay dpy, EGLint const *attrs, int32_t format,
                                              EGLConfig *outConfig) {
    auto logger = Log("NativeGL.main");
    EGLint numConfigs = -1, n = 0;
    if (!attrs)
        return -1;
    if (outConfig == NULL)
        return -1;

    int fbSzA, fbSzR, fbSzG, fbSzB;
    switch (format) {
        case WINDOW_FORMAT_RGBA_8888:
            fbSzA = fbSzR = fbSzG = fbSzB = 8;
            break;
        case WINDOW_FORMAT_RGBX_8888:
            fbSzA = 0; fbSzR = fbSzG = fbSzB = 8;
            break;
        case WINDOW_FORMAT_RGB_565:
            fbSzA = 0; fbSzR = 5; fbSzG = 6; fbSzB = 5;
            break;
        default:
            logger.w("Unknown format");
            return -1;
    }
    // Get all the "potential match" configs...
    if (eglGetConfigs(dpy, NULL, 0, &numConfigs) == EGL_FALSE) {
        logger.w("eglGetConfigs failed");
        return -1;
    }
    EGLConfig* const configs = (EGLConfig*)malloc(sizeof(EGLConfig)*numConfigs);
    if (eglChooseConfig(dpy, attrs, configs, numConfigs, &n) == EGL_FALSE) {
        free(configs);
        logger.w("eglChooseConfig failed");
        return -1;
    }
    int i;
    EGLConfig config = NULL;
    for (i=0 ; i<n ; i++) {
        EGLint r,g,b,a;
        EGLConfig curr = configs[i];
        eglGetConfigAttrib(dpy, curr, EGL_RED_SIZE,   &r);
        eglGetConfigAttrib(dpy, curr, EGL_GREEN_SIZE, &g);
        eglGetConfigAttrib(dpy, curr, EGL_BLUE_SIZE,  &b);
        eglGetConfigAttrib(dpy, curr, EGL_ALPHA_SIZE, &a);
        if (fbSzA == a && fbSzR == r && fbSzG == g && fbSzB  == b) {
            config = curr;
            break;
        }
    }
    free(configs);

    if (i<n) {
        *outConfig = config;
        return 0;
    }
    logger.w("No config with desired pixel format");
    return -1;
}

int32_t nativelib::EGLHelper::selectConfigForNativeWindow(EGLDisplay dpy, EGLint const* attrs,
                                    EGLNativeWindowType window, EGLConfig* outConfig) {
    if (!window)
        return -1;

    return selectConfigForPixelFormat(dpy, attrs,
                                      ANativeWindow_getFormat(window), outConfig);
}

void nativelib::EGLHelper::checkMaxVertexAttribs() {
    GLint vertexAttribs;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &vertexAttribs);

    auto logger = Log("NativeGL.main");
    logger.i("Max vertex attrs is: %d", vertexAttribs);
}
