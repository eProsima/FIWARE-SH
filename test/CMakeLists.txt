# Copyright 2019 Open Source Robotics Foundation, Inc.
# Copyright (C) 2020 - present Proyectos y Sistemas de Mantenimiento SL (eProsima).
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
# is-fiware SystemHandle library integration tests project

#########################################################################################
# CMake build rules for the Integration Service FIWARE SystemHandle tests library
#########################################################################################
include(CTest)
include(${IS_GTEST_CMAKE_MODULE_DIR}/gtest.cmake)
enable_testing()

#########################################################################################
# Integration tests
#########################################################################################
find_package(is-mock REQUIRED)

add_executable(${PROJECT_NAME}-integration-test
    integration/roundtrip.cpp
)

set_target_properties(${PROJECT_NAME}-integration-test PROPERTIES
    VERSION
        ${PROJECT_VERSION}
    SOVERSION
        ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    CXX_STANDARD
        17
    CXX_STANDARD_REQUIRED
        YES
)

target_compile_options(${PROJECT_NAME}-integration-test
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-Werror -Wall -Wextra -Wpedantic>
)

target_include_directories(${PROJECT_NAME}-integration-test
    PRIVATE
        $<TARGET_PROPERTY:${PROJECT_NAME},INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(${PROJECT_NAME}-integration-test
    PRIVATE
        is::mock
        $<$<PLATFORM_ID:Windows>:yaml-cpp>
    PUBLIC
        $<IF:$<BOOL:${IS_GTEST_EXTERNAL_PROJECT}>,libgtest,gtest>
)

target_compile_definitions(${PROJECT_NAME}-integration-test
    PRIVATE
        "FIWARE__IP__TEST_CONFIG=\"127.0.0.1\""
        "FIWARE__PORT__TEST_CONFIG=\"1026\""
)

add_gtest(${PROJECT_NAME}-integration-test SOURCES integration/roundtrip.cpp)

# Windows dll dependencies installation
if(WIN32)
    find_file(MOCKDLL NAMES "is-mock.dll" PATHS "${is-mock_DIR}" PATH_SUFFIXES "lib" )
    install (FILES ${MOCKDLL} DESTINATION "${CMAKE_INSTALL_PREFIX}/../bin")
endif()
