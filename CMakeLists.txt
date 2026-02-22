cmake_minimum_required(VERSION 3.16)
project(OmniGraph)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_library(ODBC_LIBRARY odbc)
if(NOT ODBC_LIBRARY)
    message(FATAL_ERROR "ODBC library not found")
endif()

find_package(Boost REQUIRED COMPONENTS thread)

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found")
endif()

find_package(OpenSSL REQUIRED)

find_library(BOOST_JSON_LIB boost_json)
if(NOT BOOST_JSON_LIB)
    message(WARNING "Boost.JSON library not found, assuming header-only")
    set(BOOST_JSON_LIB "")
endif()

message(STATUS "Boost version: ${Boost_VERSION}")
message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIRS}")
message(WARNING "Boost libraries: ${Boost_LIBRARIES}")

find_library(SODIUM_LIB sodium REQUIRED)
if(NOT SODIUM_LIB)
    message(FATAL_ERROR "libsodium not found")
endif()

find_package(Threads REQUIRED)

find_library(GRAPHQLRESPONSE_LIB NAMES graphqlresponse REQUIRED)
find_library(GRAPHQLPEG_LIB NAMES graphqlpeg REQUIRED)
find_library(GRAPHQLSERVICE_LIB NAMES graphqlservice REQUIRED)

find_path(GRAPHQL_INCLUDE_DIR NAMES graphqlservice/GraphQLService.h REQUIRED)

set(GRAPHQL_GENERATED_DIR ${CMAKE_CURRENT_SOURCE_DIR}/GraphQL/Generated)
set(GRAPHQL_RESOLVERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/GraphQL/Resolvers)
set(GRAPHQL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/GraphQL)

file(GLOB_RECURSE GRAPHQL_GENERATED_SOURCES
        ${GRAPHQL_GENERATED_DIR}/*.cpp
)

file(GLOB_RECURSE GRAPHQL_RESOLVERS_SOURCES
        ${GRAPHQL_RESOLVERS_DIR}/*.cpp
)

set(PROJECT_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Configuration.cpp
)

add_executable(OmniGraph
        ${GRAPHQL_GENERATED_SOURCES}
        ${GRAPHQL_RESOLVERS_SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/HTTPServer.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GraphQLEngine.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/HTTPServer.cpp
        ${PROJECT_SOURCES}
)

set_target_properties(OmniGraph PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/OmniGraph"
)

    target_include_directories(OmniGraph PRIVATE
        ${Boost_INCLUDE_DIRS}
        ${GRAPHQL_INCLUDE_DIR}
        ${GRAPHQL_DIR}
        ${GRAPHQL_GENERATED_DIR}
        ${GRAPHQL_RESOLVERS_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/OmniCore/Include
        ${CMAKE_CURRENT_SOURCE_DIR}/OmniCore/Include/Base
        ${CMAKE_CURRENT_SOURCE_DIR}/OmniCore/Include/User
        ${CMAKE_CURRENT_SOURCE_DIR}/OmniCore/Include/Session
        ${CMAKE_CURRENT_SOURCE_DIR}/OmniCore/Include/GlobalConfiguration
        ${CMAKE_CURRENT_SOURCE_DIR}/OmniUtils/Include
        ${CMAKE_CURRENT_SOURCE_DIR}/OmniUtils/Include/OmniUtils
        ${CMAKE_CURRENT_SOURCE_DIR}/OmniData/Include
        ${CMAKE_CURRENT_SOURCE_DIR}/OmniERP/Include
    )

if(APPLE)
    target_link_libraries(OmniGraph PRIVATE
            Threads::Threads
            ${ODBC_LIBRARY}
            ${SODIUM_LIB}
            ${Boost_LIBRARIES}
            ${BOOST_JSON_LIB}
            ${GRAPHQLPEG_LIB}
            ${GRAPHQLRESPONSE_LIB}
            ${GRAPHQLSERVICE_LIB}
            ${CMAKE_CURRENT_SOURCE_DIR}/libOmniCore.dylib
            ${CMAKE_CURRENT_SOURCE_DIR}/libOmniUtils.dylib
            iconv
    )
else()
    target_link_options(OmniGraph PRIVATE "-fuse-ld=lld")
    get_filename_component(GRAPHQL_LIB_DIR "${GRAPHQLPEG_LIB}" DIRECTORY)
    target_link_directories(OmniGraph PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/OmniCore"
        "${CMAKE_CURRENT_SOURCE_DIR}/OmniData"
        "${CMAKE_CURRENT_SOURCE_DIR}/OmniERP"
        "${CMAKE_CURRENT_SOURCE_DIR}/OmniUtils"
        "${GRAPHQL_LIB_DIR}"
    )

    target_link_libraries(OmniGraph PRIVATE
            Threads::Threads
            ${ODBC_LIBRARY}
            ${SODIUM_LIB}
            ${Boost_LIBRARIES}
            ${BOOST_JSON_LIB}
            graphqlpeg
            graphqlresponse
            graphqlservice
            OmniCore
            OmniData
            OmniERP
            OmniUtils
            ws2_32
            wsock32
            bcrypt
            OpenSSL::SSL
            OpenSSL::Crypto
    )

    add_custom_command(TARGET OmniGraph POST_BUILD
        # Copy OmniCore DLLs
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/OmniCore/Libs/libOmniCore.dll"
            "$<TARGET_FILE_DIR:OmniGraph>"
        
        # Copy OmniData DLLs
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/OmniData/Libs/libOmniData.dll"
            "$<TARGET_FILE_DIR:OmniGraph>"

        # Copy OmniERP DLLs
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/OmniERP/Libs/libOmniERP.dll"
            "$<TARGET_FILE_DIR:OmniGraph>"

        # Copy OmniUtils DLLs
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/OmniUtils/Libs/libOmniUtils.dll"
            "$<TARGET_FILE_DIR:OmniGraph>"
        
        # Copy Dependencies (Boost, Sodium)
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/OmniUtils/Libs/libboost_container-mt.dll"
            "$<TARGET_FILE_DIR:OmniGraph>"
        
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/OmniUtils/Libs/libboost_json-mt.dll"
            "$<TARGET_FILE_DIR:OmniGraph>"
        
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/OmniUtils/Libs/libsodium-26.dll"
            "$<TARGET_FILE_DIR:OmniGraph>"
        
        COMMENT "Copying DLLs to executable directory"
    )
endif()
