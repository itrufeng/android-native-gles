//
// Created by Jian Zhang on 5/26/20.
//

#include "Engine.h"
#include <GLES3/gl3.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "Log.h"

nativelib::Engine::Engine(struct android_app* app) {
    this->app = app;
    logger = new Log("NativeGL.main");
}

int nativelib::Engine::init_display() {
    // initialize OpenGL and EGL
    EGLint majorVersion;
    EGLint minorVersion;
    EGLBoolean eglInitializeState;

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitializeState = eglInitialize(display, &majorVersion, &minorVersion);
    this->display = display;

    auto logger = Log("NativeGL:main");
    logger.i("EGL initial result: %d & version: %d.%d", eglInitializeState, majorVersion, minorVersion);

    EGLConfig config;
    const EGLint attrs[] = {
            EGL_DEPTH_SIZE, 16,
            EGL_NONE
    };
    selectConfigForNativeWindow(display, attrs, this->app->window, &config);

    EGLSurface surface;
    surface = eglCreateWindowSurface(display, config, this->app->window, NULL);
    this->surface = surface;

    EGLContext context;
    const EGLint GLES3[] = {
            EGL_CONTEXT_CLIENT_VERSION, 3,
            EGL_NONE
    };
    context = eglCreateContext(display, config, NULL, GLES3);
    this->context = context;

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        logger.e("Unable to eglMakeCurrent");
        return -1;
    }

    checkMaxVertexAttribs();

    return 0;
}

/**
 * Tear down the EGL context currently associated with the display.
 */
void nativelib::Engine::destroy_display() {
    if (this->display != EGL_NO_DISPLAY) {
        eglMakeCurrent(this->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (this->context != EGL_NO_CONTEXT) {
            eglDestroyContext(this->display, this->context);
        }
        if (this->surface != EGL_NO_SURFACE) {
            eglDestroySurface(this->display, this->surface);
        }
        eglTerminate(this->display);
    }
    this->display = EGL_NO_DISPLAY;
    this->context = EGL_NO_CONTEXT;
    this->surface = EGL_NO_SURFACE;
}

/**
 * Just the current frame in the display.
 */
void nativelib::Engine::draw_frame() {
    if (this->display == NULL) {
        // No display.
        return;
    }
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    eglSwapBuffers(this->display, this->surface);
}

void nativelib::Engine::prepare() {
    logger->i("OpenGLES only support version: %s", glGetString(GL_VERSION));
    logger->i("OpenGLES shader only support version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    char vertex_shader_source_buffer[1024];
    memset(vertex_shader_source_buffer, '\0', 1024);
    this->read_shader_source("shader.vert", vertex_shader_source_buffer);
    const GLchar* vertex_shader_source = vertex_shader_source_buffer;

    GLuint vertex_shader;
    this->CreateShader(&vertex_shader_source, GL_VERTEX_SHADER, &vertex_shader);

    char fragment_shader_source_buffer[1024];
    memset(fragment_shader_source_buffer, '\0', 1024);
    this->read_shader_source("shader.frag", fragment_shader_source_buffer);
    GLchar* fragment_shader_source = fragment_shader_source_buffer;

    GLuint fragment_shader;
    this->CreateShader(&fragment_shader_source, GL_FRAGMENT_SHADER, &fragment_shader);

    GLuint shader_program;
    this->CreateProgram(vertex_shader, fragment_shader, &shader_program);

    glUseProgram(shader_program);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    float vertices[] = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.0f,  0.5f, 0.0f
    };

//    GLuint VBO;
//    glGenBuffers(1, &VBO);

//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(0);
}

bool nativelib::Engine::isAppWindowCreated() {
    return this->app->window != NULL;
}

void nativelib::Engine::read_shader_source(string path, char *out_source) {
    AAssetManager* mgr = this->app->activity->assetManager;
    AAsset *asset = AAssetManager_open(mgr, path.c_str(), AASSET_MODE_BUFFER);
    off64_t length = AAsset_getLength64(asset);
    AAsset_read(asset, out_source, length);
    AAsset_close(asset);
}

void nativelib::Engine::CreateShader(const GLchar *const*source, GLenum type, GLuint* shader) {
    GLint vertex_shader_compile_status = GL_FALSE;
    *shader = glCreateShader(type);
    glShaderSource(*shader, 1, source, NULL);
    glCompileShader(*shader);
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &vertex_shader_compile_status);
    if (!vertex_shader_compile_status) {
        logger->e("shader_compile_status: %d", vertex_shader_compile_status);

        GLint length = 0;
        glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &length);

        GLchar log[length];
        glGetShaderInfoLog(*shader, length, &length, log);
        logger->e("shader_compile_log: %s", log);

        glDeleteShader(*shader);
        return;
    }
}

void nativelib::Engine::CreateProgram(GLuint vertexShader, GLuint fragmentShader, GLuint *program) {
    GLint shader_program_link_status = GL_FALSE;
    *program = glCreateProgram();
    glAttachShader(*program, vertexShader);
    glAttachShader(*program, fragmentShader);
    glLinkProgram(*program);
    glGetProgramiv(*program, GL_LINK_STATUS, &shader_program_link_status);
    if (!shader_program_link_status) {
        logger->e("shader_program_link_status: %d", shader_program_link_status);

        GLint length = 0;
        glGetProgramiv(*program, GL_INFO_LOG_LENGTH, &length);

        GLchar log[length];
        glGetProgramInfoLog(*program, length, &length, log);
        logger->e("shader_program_link_status_log: %s", log);

        glDeleteProgram(*program);
        return;
    }
}

int32_t nativelib::Engine::selectConfigForPixelFormat(EGLDisplay dpy, EGLint const *attrs, int32_t format,
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

int32_t nativelib::Engine::selectConfigForNativeWindow(EGLDisplay dpy, EGLint const* attrs,
                                                          EGLNativeWindowType window, EGLConfig* outConfig) {
    if (!window)
        return -1;

    return selectConfigForPixelFormat(dpy, attrs,
                                      ANativeWindow_getFormat(window), outConfig);
}

void nativelib::Engine::checkMaxVertexAttribs() {
    GLint vertexAttribs;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &vertexAttribs);

    auto logger = Log("NativeGL.main");
    logger.i("Max vertex attrs is: %d", vertexAttribs);
}
