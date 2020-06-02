//
// Created by Jian Zhang on 5/26/20.
//

#include "Engine.h"
#include <GLES3/gl3.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "Log.h"
#include "EGLHelper.h"

nativelib::Engine::Engine() {
    helper = new EGLHelper();
}

int nativelib::Engine::init_display() {
    return helper->init_display(this);
}

/**
 * Tear down the EGL context currently associated with the display.
 */
int nativelib::Engine::destroy_display() {
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
    auto logger = Log("NativeGL.main");
    logger.i("OpenGLES only support version: %s", glGetString(GL_VERSION));
    logger.i("OpenGLES shader only support version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    char vertex_shader_source_buffer[1024];
    memset(vertex_shader_source_buffer, '\0', 1024);
    this->read_shader_source("shader.vert", vertex_shader_source_buffer);
    const GLchar* vertex_shader_source = vertex_shader_source_buffer;

    GLuint vertex_shader;
    GLint vertex_shader_compile_status = GL_FALSE;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &vertex_shader_compile_status);
    if (!vertex_shader_compile_status) {
        logger.e("vertex_shader_compile_status: %d", vertex_shader_compile_status);

        GLint length = 0;
        glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &length);

        GLchar log[length];
        glGetShaderInfoLog(vertex_shader, length, &length, log);
        logger.e("vertex_shader_compile_log: %s", log);

        glDeleteShader(vertex_shader);
        return;
    }

    char fragment_shader_source_buffer[1024];
    memset(fragment_shader_source_buffer, '\0', 1024);
    this->read_shader_source("shader.frag", fragment_shader_source_buffer);
    GLchar* fragment_shader_source = fragment_shader_source_buffer;

    GLuint fragment_shader;
    GLint fragment_shader_compile_status = GL_FALSE;
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &fragment_shader_compile_status);
    if (!fragment_shader_compile_status) {
        logger.e("fragment_shader_compile_status: %d", fragment_shader_compile_status);

        GLint length = 0;
        glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &length);

        GLchar log[length];
        glGetShaderInfoLog(fragment_shader, length, &length, log);
        logger.e("fragment_shader_compile_status_log: %s", log);

        glDeleteShader(fragment_shader);
        return;
    }

    GLuint shader_program;
    GLint shader_program_link_status = GL_FALSE;
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &shader_program_link_status);
    if (!shader_program_link_status) {
        logger.e("shader_program_link_status: %d", shader_program_link_status);

        GLint length = 0;
        glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &length);

        GLchar* log;
        glGetProgramInfoLog(shader_program, length, &length, log);
        logger.e("shader_program_link_status_log: %s", log);

        glDeleteProgram(shader_program);
        return;
    }

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

void nativelib::Engine::read_shader_source(char *path, char *out_source) {
    AAssetManager* mgr = this->app->activity->assetManager;
    AAsset *asset = AAssetManager_open(mgr, path, AASSET_MODE_BUFFER);
    off64_t length = AAsset_getLength64(asset);
    AAsset_read(asset, out_source, length);
    AAsset_close(asset);
}

