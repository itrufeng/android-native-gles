//
// Created by Jian Zhang on 5/26/20.
//

#include <android_native_app_glue.h>
#include "Engine.h"

using namespace nativelib;

/**
 * Process the next input event.
 */
static int32_t handle_input(struct android_app* app, AInputEvent* event) {
    return 0;
}

/**
 * Process the next main command.
 */
static void handle_cmd(struct android_app* app, int32_t cmd) {
    Engine* engine = (Engine*)(app->userData);
    switch (cmd) {
        case APP_CMD_SAVE_STATE:
            // The system has asked us to save our current state.  Do so.
            break;
        case APP_CMD_INIT_WINDOW:
            // The window is being shown, get it ready.
            if (engine->IsAppWindowCreated()) {
                engine->InitDisplay();
                engine->Prepare();
                engine->DrawFrame();
            }
            break;
        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, clean it up.
            engine->DestroyDisplay();
            break;
        case APP_CMD_GAINED_FOCUS:
            break;
        case APP_CMD_LOST_FOCUS:
            break;
    }
}

void android_main(struct android_app* state) {
    auto engine = new Engine(state);
    state->userData = engine;
    state->onAppCmd = handle_cmd;
    state->onInputEvent = handle_input;
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
                engine->DestroyDisplay();
                return;
            }
        }
        engine->DrawFrame();
    }
}