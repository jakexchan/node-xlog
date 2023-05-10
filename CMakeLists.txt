cmake_minimum_required(VERSION 3.6)

# cmake_policy(SET CMP0042 NEW)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

# set(XLOG_LIB_DIR mars/mars/log)
# set(XLOG_LIB_NAME log)
IF(APPLE)
  add_compile_definitions(FORCE_POSIX)
ENDIF()

if(MSVC AND CMAKE_JS_NODELIB_DEF AND CMAKE_JS_NODELIB_TARGET)
  # Generate node.lib
  execute_process(COMMAND ${CMAKE_AR} /def:${CMAKE_JS_NODELIB_DEF} /out:${CMAKE_JS_NODELIB_TARGET} ${CMAKE_STATIC_LINKER_FLAGS})
endif()

# for zstd
option(ZSTD_BUILD_STATIC "BUILD STATIC LIBRARIES" ON)
option(ZSTD_BUILD_SHARED "BUILD SHARED LIBRARIES" OFF)
set(ZSTD_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/mars/mars/zstd")
set(LIBRARY_DIR ${ZSTD_SOURCE_DIR}/lib)
include(GNUInstallDirs)

project(nodejs-xlog)

include_directories(mars)
include_directories(mars/mars)
include_directories(mars/mars/comm)
include_directories(mars/mars/comm/xlogger)

include_directories(mars/mars/log/crypt)
include_directories(mars/mars/log/crypt/micro-ecc-master)
include_directories(mars/mars/log/export_include)
include_directories(mars/mars/log)
include_directories(${CMAKE_JS_INC})

# file(GLOB SELF_TEMP_SRC_FILES RELATIVE ${PROJECT_SOURCE_DIR} src/*.cc src/*.h)
# source_group(src FILES ${SELF_TEMP_SRC_FILES})
# list(APPEND SELF_SRC_FILES ${SELF_TEMP_SRC_FILES})
# file(GLOB SELF_TEMP_SRC_FILES RELATIVE ${PROJECT_SOURCE_DIR} mars/mars/comm/xlogger/*.cc mars/mars/comm/xlogger/*.c mars/mars/comm/xlogger/*.h)
# source_group(xlogger FILES ${SELF_TEMP_SRC_FILES})
# list(APPEND SELF_SRC_FILES ${SELF_TEMP_SRC_FILES})

# file(GLOB SELF_TEMP_SRC_FILES RELATIVE ${PROJECT_SOURCE_DIR} mars/mars/comm/assert/*.c mars/mars/comm/assert/*.h)
# source_group(assert FILES ${SELF_TEMP_SRC_FILES})
# list(APPEND SELF_SRC_FILES ${SELF_TEMP_SRC_FILES})
file(GLOB SOURCE_FILES "src/*.cc" "src/*.h")

# file(GLOB SOURCE_XLOGGER_FILES "mars/mars/comm/xlogger/*.cc" "mars/mars/comm/xlogger/*.c" "mars/mars/comm/xlogger/*.h")
# file(GLOB SOURCE_ASSERTS_FILES "mars/mars/comm/assert/*.c" "mars/mars/comm/assert/*.h")
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_JS_SRC})

add_subdirectory(mars/mars/comm)
add_subdirectory(mars/mars/log)
add_subdirectory(mars/mars/boost)
add_subdirectory(mars/mars/zstd/build/cmake/lib)

# add_subdirectory(${XLOG_LIB_DIR} ${XLOG_LIB_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")
target_link_libraries(
  ${PROJECT_NAME}
  xlog
  comm
  mars-boost
  libzstd_static
  ${CMAKE_JS_LIB}
)

# Include Node-API wrappers
execute_process(COMMAND node -p "require('node-addon-api').include"
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE NODE_ADDON_API_DIR
)
string(REGEX REPLACE "[\r\n\"]" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})

target_include_directories(${PROJECT_NAME} PRIVATE ${NODE_ADDON_API_DIR} ${CMAKE_JS_LIB})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_JS_LIB})

# define NPI_VERSION
add_definitions(-DNAPI_VERSION=8)