cmake_minimum_required(VERSION 3.15...4.0)

project(ACOTaskScheduler LANGUAGES C)


# ------------------ Compile with GUI ------------------ #
find_package(raylib)
if(raylib_FOUND)
    message(STATUS "Raylib was found!")
else()
    message(FATAL_ERROR "Raylib not found!")
endif()


add_executable(ACOTaskScheduler)

target_link_libraries(ACOTaskScheduler raylib m )
target_compile_options(ACOTaskScheduler PRIVATE -g -Wall -Werror -Wpedantic)

target_sources(ACOTaskScheduler PRIVATE 
    source/UIState.c
    source/Graph.c
    source/EntityInput.c
    source/Entity.c
    source/DrawHelper.c
    source/ACORNK.c
    source/Main.c
)
