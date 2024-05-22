cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
  SimpleAmqpClient
  VERSION 2.5.1
  DESCRIPTION "Short description"
  HOMEPAGE_URL "https://github.com/twig-energy/SimpleAmqpClient"
  LANGUAGES CXX)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

find_package(rabbitmq-c CONFIG REQUIRED)
find_package(Boost REQUIRED chrono system)
find_path(BOOST_VARIANT_INCLUDE_DIRS "boost/variant.hpp")
find_path(BOOST_ALGORITHM_INCLUDE_DIRS "boost/algorithm")
find_path(BOOST_FOREACH_INCLUDE_DIRS "boost/foreach.hpp")
find_path(RABBITMQC_INCLUDE_DIRS "amqp.h")

# ---- Declare library ----

add_library(
  SimpleAmqpClient_SimpleAmqpClient
  source/AmqpException.cpp
  source/AmqpLibraryException.cpp
  source/AmqpResponseLibraryException.cpp
  source/BasicMessage.cpp
  source/Channel.cpp
  source/ChannelImpl.cpp
  source/Envelope.cpp
  source/MessageReturnedException.cpp
  source/Table.cpp
  source/TableImpl.cpp)
add_library(SimpleAmqpClient::SimpleAmqpClient ALIAS
            SimpleAmqpClient_SimpleAmqpClient)

include(GenerateExportHeader)
generate_export_header(
  SimpleAmqpClient_SimpleAmqpClient
  BASE_NAME
  SimpleAmqpClient
  EXPORT_FILE_NAME
  export/SimpleAmqpClient/SimpleAmqpClient_export.hpp
  CUSTOM_CONTENT_FROM_VARIABLE
  pragma_suppress_c4251)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(SimpleAmqpClient_SimpleAmqpClient
                             PUBLIC SIMPLEAMQPCLIENT_STATIC_DEFINE)
endif()
set(Boost_USE_STATIC_RUNTIME OFF)

set_target_properties(
  SimpleAmqpClient_SimpleAmqpClient
  PROPERTIES CXX_VISIBILITY_PRESET hidden
             VISIBILITY_INLINES_HIDDEN YES
             VERSION "${PROJECT_VERSION}"
             SOVERSION "${PROJECT_VERSION_MAJOR}"
             EXPORT_NAME SimpleAmqpClient
             OUTPUT_NAME SimpleAmqpClient)

target_include_directories(
  SimpleAmqpClient_SimpleAmqpClient ${warning_guard}
  PUBLIC "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>")

target_include_directories(
  SimpleAmqpClient_SimpleAmqpClient ${warning_guard} SYSTEM
  PUBLIC "$<BUILD_INTERFACE:${BOOST_VARIANT_INCLUDE_DIRS}>"
         "$<BUILD_INTERFACE:${BOOST_ALGORITHM_INCLUDE_DIRS}>"
         "$<BUILD_INTERFACE:${BOOST_FOREACH_INCLUDE_DIRS}>"
         "$<BUILD_INTERFACE:${RABBITMQC_INCLUDE_DIRS}>")

target_compile_definitions(SimpleAmqpClient_SimpleAmqpClient
                           PUBLIC SAC_SSL_SUPPORT_ENABLED)

target_compile_features(SimpleAmqpClient_SimpleAmqpClient PUBLIC cxx_std_17)

target_link_libraries(
  SimpleAmqpClient_SimpleAmqpClient
  PUBLIC rabbitmq::rabbitmq-static Boost::boost Boost::chrono Boost::system)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Developer mode ----

if(NOT SimpleAmqpClient_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
    AUTHOR_WARNING
      "Developer mode is intended for developers of SimpleAmqpClient")
endif()

include_directories(BEFORE src ${CMAKE_CURRENT_BINARY_DIR})

if(WIN32)
  set(SOCKET_LIBRARY ws2_32)
endif()

set(SAC_LIB_SRCS
    src/SimpleAmqpClient/SimpleAmqpClient.h
    src/SimpleAmqpClient/AmqpException.h
    src/AmqpException.cpp
    src/SimpleAmqpClient/Bytes.h
    src/SimpleAmqpClient/Channel.h
    src/Channel.cpp
    src/SimpleAmqpClient/ChannelImpl.h
    src/ChannelImpl.cpp
    src/SimpleAmqpClient/BasicMessage.h
    src/BasicMessage.cpp
    src/SimpleAmqpClient/Util.h
    src/SimpleAmqpClient/AmqpLibraryException.h
    src/AmqpLibraryException.cpp
    src/SimpleAmqpClient/AmqpResponseLibraryException.h
    src/AmqpResponseLibraryException.cpp
    src/SimpleAmqpClient/BadUriException.h
    src/SimpleAmqpClient/ConnectionClosedException.h
    src/SimpleAmqpClient/ConsumerTagNotFoundException.h
    src/SimpleAmqpClient/MessageRejectedException.h
    src/SimpleAmqpClient/Envelope.h
    src/Envelope.cpp
    src/SimpleAmqpClient/MessageReturnedException.h
    src/MessageReturnedException.cpp
    src/SimpleAmqpClient/Table.h
    src/Table.cpp
    src/SimpleAmqpClient/TableImpl.h
    src/TableImpl.cpp)

add_library(SimpleAmqpClient ${SAC_LIB_SRCS})
target_link_libraries(
  SimpleAmqpClient ${Rabbitmqc_LIBRARY} ${SOCKET_LIBRARY} ${Boost_LIBRARIES}
  $<$<BOOL:${Boost_Dynamic_Linking_ENABLED}>:Boost::dynamic_linking>)

if(WIN32)
  if(NOT BUILD_SHARED_LIBS)
    target_compile_definitions(SimpleAmqpClient PUBLIC SimpleAmqpClient_STATIC)
  endif()

  set_target_properties(
    SimpleAmqpClient PROPERTIES VERSION ${SAC_VERSION}
                                OUTPUT_NAME SimpleAmqpClient.${SAC_SOVERSION})
else()
  set_target_properties(SimpleAmqpClient PROPERTIES VERSION ${SAC_VERSION}
                                                    SOVERSION ${SAC_SOVERSION})
endif()

# Some smoke tests:

option(ENABLE_TESTING "Enable smoke tests" OFF)

if(ENABLE_TESTING)
  enable_testing()

  set(BUILD_GTEST
      ON
      CACHE BOOL "" FORCE)
  set(BUILD_GMOCK
      OFF
      CACHE BOOL "" FORCE)
  set(INSTALL_GTEST
      OFF
      CACHE BOOL "" FORCE)

  # This only affects targets declared after this.
  set(BUILD_SHARED_LIBS OFF)

  mark_as_advanced(BUILD_GMOCK)
  mark_as_advanced(BUILD_GTEST)
  mark_as_advanced(INSTALL_GTEST)
  mark_as_advanced(gmock_build_tests)
  mark_as_advanced(gtest_build_samples)
  mark_as_advanced(gtest_build_tests)
  mark_as_advanced(gtest_disable_pthreads)
  mark_as_advanced(gtest_force_shared_crt)
  mark_as_advanced(gtest_hide_internal_symbols)

  add_subdirectory(third-party/googletest)
  add_subdirectory(testing)
endif(ENABLE_TESTING)

# Documentation generation
find_package(Doxygen COMPONENTS dot)
option(BUILD_API_DOCS "Build Doxygen API docs" ${DOXYGEN_FOUND})

if(BUILD_API_DOCS)
  if(NOT DOXYGEN_FOUND)
    message(FATAL_ERROR "Doxygen is required to build the API documentation")
  endif()
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
                 ${CMAKE_CURRENT_BINARY_DIR}/docs/Doxyfile @ONLY)

  add_custom_target(
    docs ALL
    COMMAND ${DOXYGEN_EXECUTABLE}
    VERBATIM
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs
    DEPENDS SimpleAmqpClient
    COMMENT "Generating API documentation"
    SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
endif()

include(GNUInstallDirs)

install(
  TARGETS SimpleAmqpClient
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(
  FILES src/SimpleAmqpClient/AmqpException.h
        src/SimpleAmqpClient/AmqpLibraryException.h
        src/SimpleAmqpClient/AmqpResponseLibraryException.h
        src/SimpleAmqpClient/BadUriException.h
        src/SimpleAmqpClient/BasicMessage.h
        src/SimpleAmqpClient/Channel.h
        src/SimpleAmqpClient/ConnectionClosedException.h
        src/SimpleAmqpClient/ConsumerCancelledException.h
        src/SimpleAmqpClient/ConsumerTagNotFoundException.h
        src/SimpleAmqpClient/Envelope.h
        src/SimpleAmqpClient/MessageReturnedException.h
        src/SimpleAmqpClient/MessageRejectedException.h
        src/SimpleAmqpClient/SimpleAmqpClient.h
        src/SimpleAmqpClient/Table.h
        src/SimpleAmqpClient/Util.h
        src/SimpleAmqpClient/Version.h
  DESTINATION include/SimpleAmqpClient)
