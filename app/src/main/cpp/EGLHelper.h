//
// Created by Jian Zhang on 6/1/20.
//

#ifndef ANDROID_NATIVE_GLES_EGLHELPER_H
#define ANDROID_NATIVE_GLES_EGLHELPER_H

#include <EGL/egl.h>

namespace nativelib {

    class Engine;

    class EGLHelper {
    public:
        int init_display(Engine *engine);

    private:
        int32_t selectConfigForPixelFormat(EGLDisplay dpy, EGLint const *attrs,
                                           int32_t format, EGLConfig *outConfig);

        int32_t selectConfigForNativeWindow(EGLDisplay dpy, EGLint const *attrs,
                                            EGLNativeWindowType window, EGLConfig *outConfig);
        void checkMaxVertexAttribs();
    };

}


#endif //ANDROID_NATIVE_GLES_EGLHELPER_H
