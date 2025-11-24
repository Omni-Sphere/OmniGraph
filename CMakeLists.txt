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
find_library(BOOST_JSON_LIB boost_json)
if(NOT BOOST_JSON_LIB)
    message(WARNING "Boost.JSON library not found, assuming header-only")
    set(BOOST_JSON_LIB "")
endif()

find_library(SODIUM_LIB sodium REQUIRED)
if(NOT SODIUM_LIB)
    message(FATAL_ERROR "libsodium not found")
endif()

find_package(Threads REQUIRED)

set(CPPGRAPHQLGEN_LIB_DIR /usr/local/lib)
set(GRAPHQLRESPONSE_LIB ${CPPGRAPHQLGEN_LIB_DIR}/libgraphqlresponse.a)
set(GRAPHQLPEG_LIB      ${CPPGRAPHQLGEN_LIB_DIR}/libgraphqlpeg.a)
set(GRAPHQLSERVICE_LIB  ${CPPGRAPHQLGEN_LIB_DIR}/libgraphqlservice.a)

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
        ${PROJECT_SOURCES}
)

target_include_directories(OmniGraph PRIVATE
        ${Boost_INCLUDE_DIRS}
        ${GRAPHQL_DIR}
        ${GRAPHQL_GENERATED_DIR}
        ${GRAPHQL_RESOLVERS_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/OmniCore/Base
        ${CMAKE_CURRENT_SOURCE_DIR}/OmniCore/User
        ${CMAKE_CURRENT_SOURCE_DIR}/OmniCore/Session
        ${CMAKE_CURRENT_SOURCE_DIR}/OmniCore/Database
        ${CMAKE_CURRENT_SOURCE_DIR}/OmniCore/Item
        ${CMAKE_CURRENT_SOURCE_DIR}/OmniCore/ItemGroup
        ${CMAKE_CURRENT_SOURCE_DIR}/OmniCore/ItemBrand
        ${CMAKE_CURRENT_SOURCE_DIR}/OmniCore/GlobalConfiguration
        ${CMAKE_CURRENT_SOURCE_DIR}/OmniUtils
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
            ${CMAKE_CURRENT_SOURCE_DIR}/libOmniCore.so
            ${CMAKE_CURRENT_SOURCE_DIR}/libOmniUtils.so
    )
endif()
