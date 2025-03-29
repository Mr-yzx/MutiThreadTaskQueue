cmake_minimum_required(VERSION 3.20)
project(MutiThreadTaskQueue)

# 添加线程包查找
find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0 -fno-omit-frame-pointer)
endif()

set(SRCS
    Task.cpp
    TaskQueue.cpp
    TaskQueueBase.cpp
    TimerEvent.cpp
    TaskQueueManager.cpp
    main.cpp
)

add_executable(${PROJECT_NAME} ${SRCS})

# 链接线程库
target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
    Threads::Threads
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
