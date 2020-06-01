#include <EGL/egl.h>
#include <GLES3/gl3.h>

#include <android/log.h>
#include <android/asset_manager.h>
#include <android/native_activity.h>
#include <android/asset_manager_jni.h>
#include <android_native_app_glue.h>
#include <malloc.h>
#include "Log.h"
#include "Engine.h"
#include <string>

using namespace nativelib;

void read_shader_source(Engine* engine, char* path, char* out_source) {
    AAssetManager* mgr = engine->app->activity->assetManager;
    AAsset *asset = AAssetManager_open(mgr, path, AASSET_MODE_BUFFER);
    off64_t length = AAsset_getLength64(asset);
    AAsset_read(asset, out_source, length);
    AAsset_close(asset);
}

void init(Engine* engine) {
    auto logger = Log("NativeGL.main");
    logger.i("OpenGLES only support version: %s", glGetString(GL_VERSION));
    logger.i("OpenGLES shader only support version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    char vertex_shader_source_buffer[1024];
    memset(vertex_shader_source_buffer, '\0', 1024);
    read_shader_source(engine, "shader.vert", vertex_shader_source_buffer);
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
    read_shader_source(engine, "shader.frag", fragment_shader_source_buffer);
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

/**
 * Just the current frame in the display.
 */
static void engine_draw_frame(Engine* engine) {
    if (engine->display == NULL) {
        // No display.
        return;
    }
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    eglSwapBuffers(engine->display, engine->surface);
}

/**
 * Tear down the EGL context currently associated with the display.
 */
static int engine_term_display(Engine* engine) {
    if (engine->display != EGL_NO_DISPLAY) {
        eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (engine->context != EGL_NO_CONTEXT) {
            eglDestroyContext(engine->display, engine->context);
        }
        if (engine->surface != EGL_NO_SURFACE) {
            eglDestroySurface(engine->display, engine->surface);
        }
        eglTerminate(engine->display);
    }
    engine->display = EGL_NO_DISPLAY;
    engine->context = EGL_NO_CONTEXT;
    engine->surface = EGL_NO_SURFACE;
}

/**
 * Process the next input event.
 */
static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) {
    return 0;
}

/**
 * Process the next main command.
 */
static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
    Engine* engine = (Engine*)(app->userData);
    switch (cmd) {
        case APP_CMD_SAVE_STATE:
            // The system has asked us to save our current state.  Do so.
            break;
        case APP_CMD_INIT_WINDOW:
            // The window is being shown, get it ready.
            if (engine->app->window != NULL) {
                engine->engine_init_display();
                init(engine);
                engine_draw_frame(engine);
            }
            break;
        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, clean it up.
            engine_term_display(engine);
            break;
        case APP_CMD_GAINED_FOCUS:
            break;
        case APP_CMD_LOST_FOCUS:
            break;
    }
}

void android_main(struct android_app* state) {
    auto engine = new Engine();
    // Make sure glue isn't stripped.
    app_dummy();
    state->userData = engine;
    state->onAppCmd = engine_handle_cmd;
    state->onInputEvent = engine_handle_input;
    engine->app = state;
    // loop waiting for stuff to do.
    while (1) {
        // Read all pending events.
        int events;
        struct android_poll_source* source;
        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while ((ALooper_pollAll(-1, NULL, &events,
                                      (void**)&source)) >= 0) {
            // Process this event.
            if (source != NULL) {
                source->process(state, source);
            }
            // Check if we are exiting.
            if (state->destroyRequested != 0) {
                engine_term_display(engine);
                return;
            }
        }
        engine_draw_frame(engine);
    }
}