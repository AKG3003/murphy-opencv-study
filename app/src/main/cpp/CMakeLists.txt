cmake_minimum_required(VERSION 3.22.1)

project("ndk35_opencv_study")

set(OPENCV ${CMAKE_SOURCE_DIR}/include)
message(OPENCV: ${OPENCV})

include_directories(${OPENCV})

file(GLOB SOURCES *.cpp *.c *.h *.hpp)
message(SOURCES: ${SOURCES})
# 添加头文件目录

# 设置编译参数
if (DEFINED ANDROID_ABI)
    set(STUDY_ABI ${ANDROID_ABI})
else ()
    set(STUDY_ABI "arm64-v8a")
endif ()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${CMAKE_SOURCE_DIR}/../jniLibs/${STUDY_ABI}")

include_directories(include)

add_library(${CMAKE_PROJECT_NAME} SHARED
        # List C/C++ source files with relative paths to this CMakeLists.txt.
        native-lib.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME}
        # List libraries link to the target library
        android
        log
        opencv_java3
)