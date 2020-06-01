//
// Created by Jian Zhang on 5/26/20.
//

#include "Engine.h"
#include "EGLHelper.h"

nativelib::Engine::Engine() {
    helper = new EGLHelper();
}

int nativelib::Engine::engine_init_display() {
    return helper->engine_init_display(this);
}


