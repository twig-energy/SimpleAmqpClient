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

string(REGEX MATCH "[0-9]+" _API_VERSION_MAJOR ${_API_VERSION_MAJOR})
string(REGEX MATCH "[0-9]+" _API_VERSION_MINOR ${_API_VERSION_MINOR})
string(REGEX MATCH "[0-9]+" _API_VERSION_PATCH ${_API_VERSION_PATCH})

set(SAC_APIVERSION
    ${_API_VERSION_MAJOR}.${_API_VERSION_MINOR}.${_API_VERSION_PATCH})

option(BUILD_SHARED_LIBS "Build SimpleAmqpClient as a shared library" ON)

# Force the use of static boost library for static libraries
include(CMakeDependentOption)

cmake_dependent_option(
  Boost_Dynamic_Linking_ENABLED "Enable boost dynamic linking" ON
  "BUILD_SHARED_LIBS" OFF)

if(Boost_Dynamic_Linking_ENABLED)
  set(Boost_USE_STATIC_LIBS OFF)
else()
  set(Boost_USE_STATIC_LIBS ON)
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
  PUBLIC "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
         "$<BUILD_INTERFACE:${BOOST_VARIANT_INCLUDE_DIRS}>"
         "$<BUILD_INTERFACE:${BOOST_ALGORITHM_INCLUDE_DIRS}>"
         "$<BUILD_INTERFACE:${BOOST_FOREACH_INCLUDE_DIRS}>"
         "$<BUILD_INTERFACE:${RABBITMQC_INCLUDE_DIRS}>")

target_include_directories(
  SimpleAmqpClient_SimpleAmqpClient SYSTEM
  PUBLIC "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/export>"
         "$<BUILD_INTERFACE:${BOOST_VARIANT_INCLUDE_DIRS}>"
         "$<BUILD_INTERFACE:${BOOST_ALGORITHM_INCLUDE_DIRS}>"
         "$<BUILD_INTERFACE:${BOOST_FOREACH_INCLUDE_DIRS}>"
         "$<BUILD_INTERFACE:${RABBITMQC_INCLUDE_DIRS}>")

target_compile_definitions(SimpleAmqpClient_SimpleAmqpClient PUBLIC SAC_SSL_SUPPORT_ENABLED)

target_compile_features(SimpleAmqpClient_SimpleAmqpClient PUBLIC cxx_std_17)

target_link_libraries(
  SimpleAmqpClient_SimpleAmqpClient
  PUBLIC rabbitmq::rabbitmq-static Boost::boost Boost::chrono Boost::system)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Developer mode ----

if(CMAKE_CXX_FLAGS STREQUAL "" AND NOT DEFINED SAC_CXX_FLAGS_SET)
  if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS
        "-Wall -Wextra"
        CACHE STRING "Flags used by the compiler during all build types." FORCE)
  endif()
  set(SAC_CXX_FLAGS_SET
      TRUE
      CACHE INTERNAL "Have the SAC default compiler flags been set?")
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

set(prefix ${CMAKE_INSTALL_PREFIX})
set(exec_prefix "\${prefix}")
set(libdir "\${prefix}/${CMAKE_INSTALL_LIBDIR}")
set(includedir "\${prefix}/include")
if(WIN32)
  get_target_property(SIMPLEAMQPCLIENT_LIB SimpleAmqpClient OUTPUT_NAME)
else(WIN32)
  set(SIMPLEAMQPCLIENT_LIB SimpleAmqpClient)
endif(WIN32)

# Propagate package dependencies
if(BUILD_SHARED_LIBS)
  set(requires_private "librabbitmq")
else(BUILD_SHARED_LIBS)
  set(requires_public "librabbitmq")
endif(BUILD_SHARED_LIBS)

# Propagate interface compile definitions
set(SIMPLEAMQPCLIENT_DEFINITIONS "")
get_target_property(propagated_definitions SimpleAmqpClient
                    INTERFACE_COMPILE_DEFINITIONS)
if(propagated_definitions)
  foreach(_def ${propagated_definitions})
    set(SIMPLEAMQPCLIENT_DEFINITIONS
        "${SIMPLEAMQPCLIENT_DEFINITIONS} -D${_def}")
  endforeach()
endif(propagated_definitions)

# Propagate library dependencies
set(libs_private "")
set(libs_public "")

if(BUILD_SHARED_LIBS)
  set(populate_libs "libs_private")
else(BUILD_SHARED_LIBS)
  set(populate_libs "libs_public")
  set(extra_win32_targets "${Rabbitmqc_LIBRARY};${SOCKET_LIBRARY}")
endif(BUILD_SHARED_LIBS)

foreach(_lib ${Boost_LIBRARIES} ${extra_win32_targets})

  # Check if FindBoost.cmake provided actual library paths or targets
  if(TARGET ${_lib})
    get_target_property(_lib ${_lib} LOCATION)
    message(WARNING "Using target ${_lib} as a library")
  endif()

  get_filename_component(_LIBPATH ${_lib} PATH)
  if(NOT _LIBPATH STREQUAL _LASTLIBPATH AND NOT _LIBPATH STREQUAL "")
    set(${populate_libs} "${${populate_libs}} -L\"${_LIBPATH}\"")
    set(_LASTLIBPATH ${_LIBPATH})
  endif()

  get_filename_component(_LIBNAME ${_lib} NAME_WLE)
  if(NOT _LIBNAME STREQUAL "debug" AND NOT _LIBNAME STREQUAL "optimized")
    if(NOT WIN32)
      string(REGEX REPLACE "^lib" "" _LIBNAME ${_LIBNAME})
    endif()
    set(_LIBNAME "-l${_LIBNAME}")
    set(${populate_libs} "${${populate_libs}} ${_LIBNAME}")
  endif()
endforeach()

configure_file(libSimpleAmqpClient.pc.in
               ${CMAKE_CURRENT_BINARY_DIR}/libSimpleAmqpClient.pc @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libSimpleAmqpClient.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
