# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library(
        native-lib

        SHARED

        main.c
        glutils.c
)

include_directories(
        ${ANDROID_NDK}/sources/android/native_app_glue
        ${COMMON_SOURCE_DIR}
)

add_library(
        android_native_app_glue

        STATIC

        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
)
set(
        CMAKE_SHARED_LINKER_FLAGS
        "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate"
)

target_link_libraries(
        native-lib

        android
        log
        EGL
        GLESv2
        android_native_app_glue
)