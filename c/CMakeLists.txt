cmake_minimum_required(VERSION 3.23)
project(dontkillsanta)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

##########################################################################################
# Configurable options
##########################################################################################

# toggle libs to be used with ON and OFF
option(USE_RAYLIB "Use raylib" ON)
option(USE_MINIMAL_RAYLIB_MODULES "Use minimal raylib modules" OFF)

##########################################################################################
# Platform Configuration
##########################################################################################

if (NOT DEFINED PLATFORM)
    set(PLATFORM "default")
endif()

##########################################################################################
# Make compile_commands.json available at top leve
##########################################################################################
if (PROJECT_IS_TOP_LEVEL AND UNIX)
    # Symlink to compile_commands.json for easy editor use
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            ${CMAKE_BINARY_DIR}/compile_commands.json
            ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
    )
endif()

##########################################################################################
# Add dependencies with FetchContent
##########################################################################################

# libraries array to be linked
set(LIBRARIES ${PROJECT_NAME})

include(FetchContent)
function(add_git_dependency libName gitURL gitTag)

        FetchContent_Declare(${libName}
            GIT_REPOSITORY ${gitURL}
            GIT_TAG        ${gitTag}
            GIT_SHALLOW    TRUE
            GIT_PROGRESS   TRUE
        )

        if(NOT ("${libName}" STREQUAL "libname"))
            FetchContent_MakeAvailable(${libName})
        endif()

        # exclude lib to avoid errors
        if(NOT "${libName}" STREQUAL "libname" AND NOT "${libName}" STREQUAL "rlImGui")
            # target_compile_options(${libName} PRIVATE "-w")
        endif()
endfunction()

##########################################################################################
# Raylib Setup
##########################################################################################

if(USE_RAYLIB)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
    set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # don't build the supplied example games
    set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
    set(CUSTOMIZE_BUILD OFF CACHE BOOL "" FORCE) 

    set(dep raylib)
    add_git_dependency(
        ${dep}
        https://github.com/raysan5/raylib.git
        c1ab645ca298a2801097931d1079b10ff7eb9df8 # v5.5
    )

    FetchContent_GetProperties(${dep})
    if(NOT ${dep}_POPULATED)
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/external/raylib/config.h ${CMAKE_CURRENT_SOURCE_DIR}/build/_deps/raylib-src/src/config.h COPYONLY)
        FetchContent_Populate(${dep})

        add_subdirectory(${${dep}_SOURCE_DIR} ${${dep}_BINARY_DIR})
    endif()

    # add the dependency here to be linked
    list(APPEND LIBRARIES ${dep})
endif()

##########################################################################################
# Project executable setup
##########################################################################################

# Adding our source files
# Define PROJECT_SOURCES as a list of all source files
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/src/*.c")

# Define PROJECT_INCLUDE to be the path to the include directory of the project
set(PROJECT_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/include/")

# Declaring our executable
add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_INCLUDE})

##########################################################################################
# Compiler and linker flags
##########################################################################################

if(NOT ${PLATFORM} STREQUAL "Web")
    if (CMAKE_C_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -pthread")
        target_link_libraries(${LIBRARIES} -static-libgcc -Wl,-Bstatic -lpthread -Wl,-Bdynamic)
        if (CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-rtti")
        endif()
    elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU")
        target_link_libraries(${LIBRARIES} -static-libgcc -Wl,-Bstatic -lpthread -Wl,-Bdynamic)
    elseif (CMAKE_C_COMPILER_ID STREQUAL "Intel")
        target_link_libraries(${LIBRARIES} -static-intel -Wl,-Bstatic -lpthread -Wl,-Bdynamic)
    elseif (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
        target_link_libraries(${LIBRARIES})

        if(OFF)
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
        endif()

        if (CMAKE_BUILD_TYPE STREQUAL "Debug")
            message("-- console enabled")
            set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
        else()
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
        endif()
    else()
        target_link_libraries(${LIBRARIES})
    endif()
else()
    target_link_libraries(${LIBRARIES})
endif()

##########################################################################################
# Build flags and additional settings
##########################################################################################

# copy assets over into the build dir for the executable to have them for debug/run reasons
if(NOT USE_NO_ASSETS)
    add_custom_target(copyAssetsOver
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        ${CMAKE_CURRENT_BINARY_DIR}/assets
        COMMENT "Copying assets to build directory"
    )
    add_dependencies(${PROJECT_NAME} copyAssetsOver)
endif()

add_definitions(-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})

# for web build when using emscripten
if(ASYNCIFY)
  # Add Raylib's async support
  add_definitions(-DRAYLIB_ASYNC_ENABLE)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s ASYNCIFY")

  # Add the -O3 optimization flag
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
endif()

# We only need the special raylib build if we are building for web.
if(USE_RAYLIB AND PLATFORM STREQUAL "Web")
    # Function to toggle a define in a given config file
    function(toggle_define file_path define)
        file(READ "${file_path}" input_file_contents)
        string(REPLACE "// #define ${define}" "#define ${define}" modified_contents "${input_file_contents}")
        file(WRITE "${file_path}" "${modified_contents}")
    endfunction()

    # used to make sure all modules are turned back on
    set(module_strings
        "SUPPORT_MODULE_RSHAPES"
        "SUPPORT_MODULE_RTEXTURES" 
        "SUPPORT_MODULE_RTEXT"
        "SUPPORT_MODULE_RMODELS"
        "SUPPORT_MODULE_RAUDIO"
    )

    # Toggle each define in the config file
    foreach(currentString ${module_strings})
        toggle_define("${CMAKE_CURRENT_SOURCE_DIR}/build/_deps/raylib-src/src/config.h" ${currentString})
    endforeach()

    # enusre raylib is rebuilt with new config.h
    add_custom_command(
        TARGET ${PROJECT_NAME}
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} .
    )
endif()

# Setting ASSETS_PATH
if (CMAKE_BUILD_TYPE MATCHES "Debug")
    SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -O0 -DDEBUG")
    message("-- console enabled")
    if(NOT USE_NO_ASSETS AND NOT ${PLATFORM} STREQUAL "Web")
        # Set the asset path macro to the absolute path on the dev machine
        target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/")
    endif()
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
    if(NOT USE_NO_ASSETS AND NOT ${PLATFORM} STREQUAL "Web")
        # Set the asset path macro in release mode to a relative path that assumes the assets folder is in the same directory as the game executable
        target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="./assets/")
    endif()
    if(WIN32)
      if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows") 
      endif()
      add_compile_definitions(NDEBUG)
    endif()
endif()

# Set common compiler flags
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

if (${PLATFORM} STREQUAL "Web")
    # Tell Emscripten to build an .html file.
    SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Os")
    set(CMAKE_EXECUTABLE_SUFFIX ".html") # This line is used to set your executable to build with the emscripten html template so that you can directly open it.
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
    if(NOT USE_NO_ASSETS)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -Os -Wall -s TOTAL_MEMORY=67108864 -s FORCE_FILESYSTEM=1 --preload-file assets/ --shell-file ../vendor/minshell.html")
        target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="/assets/") # Set the asset path macro in release mode to a relative path that assumes the assets folder is in the same directory as the game executable
    else()
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -Os -Wall -s TOTAL_MEMORY=67108864 -s FORCE_FILESYSTEM=1 --shell-file ../vendor/minshell.html")
    endif()
endif()
