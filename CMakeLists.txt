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

find_library(BOOST_JSON_LIB boost_json)
if(NOT BOOST_JSON_LIB)
    message(WARNING "Boost.JSON library not found, assuming header-only")
    set(BOOST_JSON_LIB "")
endif()

message(STATUS "Boost version: ${Boost_VERSION}")
message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost libraries: ${Boost_LIBRARIES}")

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
)

add_executable(OmniGraph
        ${GRAPHQL_GENERATED_SOURCES}
        ${GRAPHQL_RESOLVERS_SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/GraphQLEngine.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/HTTPServer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/CDNServer.cpp
        ${PROJECT_SOURCES}
)

    target_include_directories(OmniGraph PRIVATE
        ${Boost_INCLUDE_DIRS}
        ${GRAPHQL_INCLUDE_DIR}
        ${GRAPHQL_DIR}
        ${GRAPHQL_GENERATED_DIR}
        ${GRAPHQL_RESOLVERS_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/OmniCore/Include
        ${CMAKE_CURRENT_SOURCE_DIR}/OmniCore/Include/OmniCore/Base
        ${CMAKE_CURRENT_SOURCE_DIR}/OmniCore/Include/OmniCore/User
        ${CMAKE_CURRENT_SOURCE_DIR}/OmniCore/Include/OmniCore/Session
        ${CMAKE_CURRENT_SOURCE_DIR}/OmniCore/Include/OmniCore/Database
        ${CMAKE_CURRENT_SOURCE_DIR}/OmniCore/Include/OmniCore/Item
        ${CMAKE_CURRENT_SOURCE_DIR}/OmniCore/Include/OmniCore/ItemGroup
        ${CMAKE_CURRENT_SOURCE_DIR}/OmniCore/Include/OmniCore/ItemBrand
        ${CMAKE_CURRENT_SOURCE_DIR}/OmniCore/Include/OmniCore/GlobalConfiguration
        ${CMAKE_CURRENT_SOURCE_DIR}/OmniUtils/Include
        ${CMAKE_CURRENT_SOURCE_DIR}/OmniUtils/Include/OmniUtils
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
    target_link_libraries(OmniGraph PRIVATE
            Threads::Threads
            ${ODBC_LIBRARY}
            ${SODIUM_LIB}
            ${Boost_LIBRARIES}
            ${BOOST_JSON_LIB}
            ${GRAPHQLPEG_LIB}
            ${GRAPHQLRESPONSE_LIB}
            ${GRAPHQLSERVICE_LIB}
            ${CMAKE_CURRENT_SOURCE_DIR}/OmniCore/Libs/libOmniCore.dll.a
            ${CMAKE_CURRENT_SOURCE_DIR}/OmniUtils/Libs/libOmniUtils.dll.a
    )
endif()
