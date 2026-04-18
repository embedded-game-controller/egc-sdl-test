cmake_minimum_required(VERSION 3.13)
project (EgcSdlTest VERSION 1.0)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL REQUIRED IMPORTED_TARGET embedded-game-controller sdl2 SDL2_ttf freetype2 opengl glu)

set(PROG "EgcSdlTest")

function(add_resource out_var)
    set(result)
    foreach(in_f ${ARGN})
        string(MAKE_C_IDENTIFIER ${in_f} input_identifier)
        set(out_f "${CMAKE_CURRENT_BINARY_DIR}/${input_identifier}.o")

        add_custom_command(
            OUTPUT ${out_f}
            COMMAND ${CMAKE_LINKER} --relocatable --format binary --output ${out_f} ${in_f}
            DEPENDS ${in_f}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Embedding ${in_f} as a resource"
        )
        list(APPEND result ${out_f})
    endforeach()
    set(${out_var} ${result} PARENT_SCOPE)
endfunction()

# Resource file list
#add_resource(IMAGES "images.data")
#add_resource(FONTS "mario286.ttf")

add_executable(${PROG}
    src/button-big.c
    src/button-dpad.c
    src/button.c
    src/controller.c
    src/joystick.c  
    src/main.c
    src/shoulder.c
    src/start.c
    src/trigger.c
    ${IMAGES}
    ${FONTS}
)

target_link_libraries(${PROG} PUBLIC
    PkgConfig::SDL
    m
)


if (NINTENDO_WII)
    ogc_create_dol(${PROG})
endif()
