cmake_minimum_required(VERSION 3.28)

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
find_package(Boost REQUIRED COMPONENTS algorithm chrono foreach lexical_cast system variant)

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

target_compile_definitions(SimpleAmqpClient_SimpleAmqpClient
                           PUBLIC SAC_SSL_SUPPORT_ENABLED)

target_compile_features(SimpleAmqpClient_SimpleAmqpClient PUBLIC cxx_std_17)

target_link_libraries(
  SimpleAmqpClient_SimpleAmqpClient
  PUBLIC rabbitmq::rabbitmq-static Boost::algorithm Boost::boost Boost::chrono Boost::foreach Boost::lexical_cast Boost::system Boost::variant)

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

include(cmake/dev-mode.cmake)
