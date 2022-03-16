# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=return-type -fPIC")
set(jnilibs "${CMAKE_SOURCE_DIR}/../jniLibs")

if(ANDROID)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_USE_CXX11_ABI=0 -DANDROID_STL=c++_shared -stdlib=libc++ -Wno-gnu-zero-variadic-macro-arguments -Werror=return-type")
endif()


link_directories(${CMAKE_BINARY_DIR}/install/lib)
include_directories(
        util
        render
        sample
        inc
        inc/assimp
        inc/freetype_2_9_1
        model
        egl
        looper
)
if(ANDROID)
    include_directories(
            opencv_3_0_0
    )
else()
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)

if(ANDROID)
file(GLOB src-files
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/render/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/util/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/sample/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/egl/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/looper/*.cpp)
else()
    file(GLOB src-files
            ${CMAKE_CURRENT_SOURCE_DIR}/render/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/util/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/PBOSample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/AvatarSample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/BasicLightingSample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/BeatingHeartSample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/BezierCurveSample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/BigEyesSample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/BigHeadSample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/BlendingSample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/CloudSample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/ConveyorBeltExample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/CoordSystemSample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/DepthTestingSample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/FaceSlenderSample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/FBOBlitSample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/FBOLegLengthenSample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/FBOSample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/GLTransitionExample_2.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/GLTransitionExample_3.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/GLTransitionExample_4.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/GLTransitionExample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/Instancing3DSample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/InstancingSample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/Model3DSample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/MRTSample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/MultiLightsSample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/Noise3DSample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/NV21TextureMapSample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/ParticlesSample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/PBOSample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/PortraitStayColorExample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/RGB2I420Sample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/RGB2NV21Sample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/RGB2YUYVSample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/RotaryHeadSample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/ScratchCardSample.cpp
#            ${CMAKE_CURRENT_SOURCE_DIR}/sample/SharedEGLContextSample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/ShockWaveSample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/SkyBoxSample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/StencilTestingSample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/TextRenderSample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/TextureBuffferSample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/TextureMapSample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/TimeTunnelSample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/TransformFeedbackSample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/TriangleSample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/UniformBufferSample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/VaoSample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/VisualizeAudioSample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/YUVP010Example.cpp

            ${CMAKE_CURRENT_SOURCE_DIR}/egl/*.cpp
            )
endif()


add_library(native-render SHARED
        # Provides a relative path to your source file(s).
        ${src-files}
        ${CMAKE_CURRENT_SOURCE_DIR}/Components/Transform.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Components/Camera.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Components/Light.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Components/Renderer/LineRenderer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Components/Renderer/PointRenderer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Components/Renderer/TexturedMeshRenderer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Components/Renderer/LightedMeshRenderer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Components/Renderer/UIRectRenderer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Components/ObjLoader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Components/PBOCanvas.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Entity/GameObject.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/System/Scene.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Components/GLESHelpers/OwnedResources.cpp
        )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

if(ANDROID)
find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)
endif()

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

if(ANDROID)
    target_link_libraries( # Specifies the target library.
            native-render
            EGL
            log-lib
            android
            ${third-party-libs}
            GLESv2
            GLESv3
            )
else()
    find_package(OpenGL REQUIRED)
    find_package( OpenCV REQUIRED )
    find_package(Freetype REQUIRED)
    include_directories( ${OpenCV_INCLUDE_DIRS} )
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(assimp REQUIRED IMPORTED_TARGET assimp)

    target_link_libraries( # Specifies the target library.
            native-render
            PUBLIC
            # Links the target library to the log library
            ${OPENGL_gl_LIBRARY}            # included in the NDK.
            ${OPENGL_egl_LIBRARY}            # included in the NDK.
            ${OpenCV_LIBRARIES}
            ${FREETYPE_LIBRARIES}
            PkgConfig::assimp
            )
endif()


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
        UPDATE_COMMAND ""
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
        UPDATE_COMMAND ""
        )

add_dependencies(native-render handycpp1)
add_dependencies(native-render happly)
target_include_directories(native-render PRIVATE ${CMAKE_BINARY_DIR}/handycpp/src/include)
target_include_directories(native-render PRIVATE ${CMAKE_BINARY_DIR}/happly/src)

#message(FATAL_ERROR "cmake binary dir ${CMAKE_BINARY_DIR}/handycpp/build")

message("CMAKE_BINARY_DIR ${CMAKE_BINARY_DIR}")

target_link_libraries(native-render PUBLIC handycpp.a)
target_compile_definitions(native-render PRIVATE "-DRENDERER_SOURCE=\"${CMAKE_CURRENT_SOURCE_DIR}\" ")
#target_link_libraries(native-render ${CMAKE_BINARY_DIR}/handycpp/build/libhandycpp.a)
