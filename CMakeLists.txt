cmake_minimum_required(VERSION 3.20)

project(VOXEL VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(Stb REQUIRED)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.cc"
    "${CMAKE_SOURCE_DIR}/src/*.cxx"
)

add_executable(voxeler ${SOURCES})

set_target_properties(voxeler PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY
        "${CMAKE_BINARY_DIR}/bin"
)

target_include_directories(voxeler PRIVATE
    ${CMAKE_SOURCE_DIR}
    "${CMAKE_SOURCE_DIR}/src"
    ${Stb_INCLUDE_DIR}
)

if(EXISTS "${CMAKE_SOURCE_DIR}/include")
    target_include_directories(voxeler PRIVATE
        "${CMAKE_SOURCE_DIR}/include"
    )
endif()

target_link_libraries(voxeler PRIVATE
    opengl32
    glad::glad
    glfw
    glm::glm
    imgui::imgui
)

if(EXISTS "${CMAKE_SOURCE_DIR}/res")
    file(GLOB_RECURSE TEST_RESOURCES
        CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/res/*"
    )

    set(RESOURCE_STAMP
        "${CMAKE_CURRENT_BINARY_DIR}/resources.stamp"
    )

    add_custom_command(
        OUTPUT "${RESOURCE_STAMP}"

        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_SOURCE_DIR}/res"
                "$<TARGET_FILE_DIR:voxeler>/../res"
        
        COMMAND ${CMAKE_COMMAND} -E touch
                "${RESOURCE_STAMP}"
        
        DEPENDS ${TEST_RESOURCES}
    )

    add_custom_target("resources"
        DEPENDS ${RESOURCE_STAMP}
    )

    add_dependencies(
        voxeler
        "resources"
    )
endif()

target_compile_options(voxeler PRIVATE
    $<$<CONFIG:Release>:
        -O3
        -ffast-math
        -march=native
    >

    $<$<CONFIG:RelWithDebInfo>:
        -O2
        -g
    >

    $<$<CONFIG:Debug>:
        -g
    >
)