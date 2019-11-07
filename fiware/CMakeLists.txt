#
# Copyright 2019 Proyectos y Sistemas de Mantenimiento SL (eProsima).
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

cmake_minimum_required(VERSION 3.5.0)

project(soss-fiware)

find_package(xtypes REQUIRED)
find_package(soss-core REQUIRED)

if(WIN32)
    find_package(yaml-cpp REQUIRED)
    include(FindCURL)
    find_package(curl REQUIRED)
endif()

########################################################################
# Find curl
find_path(CURLPP_DIR NAMES "include/curlpp/cURLpp.hpp" CMAKE_FIND_ROOT_PATH_BOTH)
find_library(CURLPP_LIBRARY NAMES curlpp PATH CURLPP_DIR)

add_library(curlpp SHARED IMPORTED)

if(WIN32)
    find_path(CURL_INCLUDE_DIRS NAMES "include/curl/curl.h")
    include_directories(
      "${CURL_INCLUDE_DIRS}"
    )

    include_directories(
      "${CURLPP_DIR}/include"
    )
endif()

set_target_properties(curlpp
    PROPERTIES
        IMPORTED_LOCATION "${CURLPP_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${CURLPP_DIR}/include"
)

if(NOT CURLPP_DIR)
    message(FATAL_ERROR "curlpp not found, please install it or set 'CMAKE_PREFIX_PATH' with a valid installation."
                        "\n\t(e.g.): sudo apt install libcurlpp-dev")
endif()

########################################################################
# Find Asio
find_path(ASIO_INCLUDE_DIR NAMES "asio.hpp")
if(NOT ASIO_INCLUDE_DIR)
    message(FATAL_ERROR "asio not found, please install it."
                        "\n\t(e.g.): sudo apt install libasio-dev")
endif()

########################################################################
# Library
set(SRC
    src/NGSIV2Connector.cpp
    src/SystemHandle.cpp
)

add_library(${PROJECT_NAME} SHARED ${SRC})

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        CXX_STANDARD 14
        POSITION_INDEPENDENT_CODE ON
        INSTALL_RPATH_USE_LINK_PATH TRUE
)

target_compile_options(${PROJECT_NAME}
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-Werror -Wall -Wextra -Wpedantic>
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        include
        ${ASIO_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        $<$<PLATFORM_ID:Windows>:${CURLPP_LIBRARY}>
        $<$<AND:$<PLATFORM_ID:Windows>,$<CONFIG:Debug>>:${CURL_LIBRARY_DEBUG}>
        $<$<AND:$<PLATFORM_ID:Windows>,$<CONFIG:Release>>:${CURL_LIBRARY_RELEASE}>
        $<$<PLATFORM_ID:Windows>:yaml-cpp>
    PRIVATE
        soss::core
        $<$<PLATFORM_ID:Linux>:curlpp>
        $<$<PLATFORM_ID:Linux>:pthread>
)

########################################################################
# Installation
soss_install_middleware_plugin(
    MIDDLEWARE fiware
    TARGET soss-fiware curlpp
)

# Windows dll dependencies installation
if(WIN32)
    find_file(CURLPPDLL NAMES "curlpp.dll" PATHS "${CURLPP_DIR}" PATH_SUFFIXES "bin" )
    find_file(CURLDDLL NAMES "libcurl-d.dll" PATHS "${CURL_INCLUDE_DIR}/../" PATH_SUFFIXES "bin" )
    find_file(CURLDLL NAMES "libcurl.dll" PATHS "${CURL_INCLUDE_DIR}/../" PATH_SUFFIXES "bin" )

    install (FILES ${CURLPPDLL} DESTINATION "${CMAKE_INSTALL_PREFIX}/../bin")
    install (FILES ${CURLDLL} DESTINATION "${CMAKE_INSTALL_PREFIX}/../bin")
    install (FILES ${CURLDDLL} DESTINATION "${CMAKE_INSTALL_PREFIX}/../bin")
endif()
