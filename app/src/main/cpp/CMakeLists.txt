# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_USE_CXX11_ABI=0 -DANDROID_STL=c++_shared -stdlib=libc++ -Wno-gnu-zero-variadic-macro-arguments")
set(jnilibs "${CMAKE_SOURCE_DIR}/../jniLibs")

link_directories(${CMAKE_BINARY_DIR}/install/lib)
include_directories(
        util
        render
        sample
        glm
        inc
        inc/assimp
        inc/freetype_2_9_1
        model
        opencv_3_0_0
        egl
        looper
)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
file(GLOB src-files
        ${CMAKE_SOURCE_DIR}/*.cpp
        ${CMAKE_SOURCE_DIR}/render/*.cpp
        ${CMAKE_SOURCE_DIR}/util/*.cpp
        ${CMAKE_SOURCE_DIR}/sample/*.cpp
        ${CMAKE_SOURCE_DIR}/egl/*.cpp
        ${CMAKE_SOURCE_DIR}/looper/*.cpp)

add_library( # Sets the name of the library.
        native-render

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        ${src-files}
        ${CMAKE_SOURCE_DIR}/Components/Transform.cpp
        ${CMAKE_SOURCE_DIR}/Components/Renderer/MeshRenderer.cpp
        ${CMAKE_SOURCE_DIR}/Components/Renderer/ObjMeshRenderer.cpp
        ${CMAKE_SOURCE_DIR}/Components/Renderer/WireFrameRenderer.cpp
        ${CMAKE_SOURCE_DIR}/Components/Renderer/TexturedMeshRenderer.cpp
        ${CMAKE_SOURCE_DIR}/Components/ObjLoader.cpp
        )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

add_library(assimp SHARED IMPORTED)
set_target_properties(assimp PROPERTIES IMPORTED_LOCATION ${jnilibs}/${ANDROID_ABI}/libassimp.so)

add_library(opencv_java3 SHARED IMPORTED)
set_target_properties(opencv_java3 PROPERTIES IMPORTED_LOCATION ${jnilibs}/${ANDROID_ABI}/libopencv_java3.so)

add_library(freetype STATIC IMPORTED)
set_target_properties(freetype PROPERTIES IMPORTED_LOCATION ${jnilibs}/${ANDROID_ABI}/libfreetype.a)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

#ASAN 需要 NDK20 编译，Android P 以上运行
#target_compile_options(native-render PUBLIC -fsanitize=address -fno-omit-frame-pointer)
#set_target_properties(native-render PROPERTIES LINK_FLAGS -fsanitize=address)

#SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fno-rtti -fno-exceptions")
set(third-party-libs
        freetype
        assimp
        opencv_java3)

target_link_libraries( # Specifies the target library.
        native-render

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib}
        ${third-party-libs}
        EGL
        GLESv2
        GLESv3
        android
        )

set(MyAndroidProject_DEFAULT_ARGS
        -DANDROID_ABI:STRING=${ANDROID_ABI}
        -DANDROID_NATIVE_API_LEVEL:STRING=${ANDROID_NATIVE_API_LEVEL}
        -DANDROID_NDK:STRING=${ANDROID_NDK}
        -DANDROID_PLATFORM:STRING=${ANDROID_PLATFORM}
        -DANDROID_STL:STRING=${ANDROID_STL}
        -DANDROID_TOOLCHAIN:STRING=${ANDROID_TOOLCHAIN}
        -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
        -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
        -DCMAKE_C_COMPILER:STRING=${CMAKE_C_COMPILER}
        -DCMAKE_CXX_COMPILER:STRING=${CMAKE_CXX_COMPILER}
        -DCMAKE_GENERATOR:STRING=${CMAKE_GENERATOR}
        -DCMAKE_MAKE_PROGRAM:FILEPATH=${CMAKE_MAKE_PROGRAM}
        -DCMAKE_TOOLCHAIN_FILE:FILEPATH=${CMAKE_TOOLCHAIN_FILE}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install
        )

include(ExternalProject)
ExternalProject_Add(handycpp1
        GIT_REPOSITORY https://github.com/zhangfuwen/handycpp
        GIT_TAG main
        CMAKE_ARGS -Wno-dev ${MyAndroidProject_DEFAULT_ARGS}
        PREFIX ${CMAKE_BINARY_DIR}
        SOURCE_DIR "${CMAKE_BINARY_DIR}/handycpp/src"
        BINARY_DIR "${CMAKE_BINARY_DIR}/handycpp/build"
        INSTALL_DIR "${CMAKE_BINARY_DIR}/install"
        #        INSTALL_COMMAND ""
        TEST_COMMAND ""
        )

ExternalProject_add(happly
        GIT_REPOSITORY https://github.com/nmwsharp/happly
        GIT_TAG master
        SOURCE_DIR "${CMAKE_BINARY_DIR}/happly/src"
        BINARY_DIR "${CMAKE_BINARY_DIR}/happly/build"
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        TEST_COMMAND ""
        )

add_dependencies(native-render handycpp1)
add_dependencies(native-render happly)
target_include_directories(native-render PRIVATE ${CMAKE_BINARY_DIR}/handycpp/src/include)
target_include_directories(native-render PRIVATE ${CMAKE_BINARY_DIR}/happly/src)

#message(FATAL_ERROR "cmake binary dir ${CMAKE_BINARY_DIR}/handycpp/build")

message("CMAKE_BINARY_DIR ${CMAKE_BINARY_DIR}")

target_link_libraries(native-render handycpp.a)
#target_link_libraries(native-render ${CMAKE_BINARY_DIR}/handycpp/build/libhandycpp.a)
