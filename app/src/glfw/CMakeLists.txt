cmake_minimum_required(VERSION 3.3)
project(LearnRenderer)

set(CMAKE_CXX_STANDARD 17) # no explicit compiler flags if possible

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -DGLFW_USE_EGL=ON ")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_USE_CXX11_ABI=0 -DANDROID_STL=c++_shared -Wno-gnu-zero-variadic-macro-arguments -Werror=return-type")

add_subdirectory(../main/cpp ${CMAKE_BINARY_DIR}/renderer)

# don't read CMAKE_BUILD_TYPE, it has no meaning with multiconfig
# generators
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/build/debug")

# PROJECT_BINARY_DIR should not be set at all
# You establish the BINARY_DIR with the initial cmake command
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

include_directories(${OPENGL_INCLUDE_DIR})

link_directories(${CMAKE_BINARY_DIR}/install/lib)
add_executable(LearnRenderer triangle.cpp)
target_link_libraries(LearnRenderer
        ${OPENGL_gl_LIBRARY}
        ${OPENGL_egl_LIBRARY}
        glfw
        native-render)
target_include_directories(LearnRenderer PRIVATE
        ../main/cpp
        ../main/cpp/inc
        ../main/cpp/opencv_3_0_0
        ${CMAKE_BINARY_DIR}/handycpp/src/include
        )
