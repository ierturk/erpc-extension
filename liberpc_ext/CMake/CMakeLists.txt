cmake_minimum_required(VERSION 3.0.0)
project(erpc)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  # Default to installing in eRPC source directory
  set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install)
endif()

set(ERPC_INCLUDE_INSTALL_DIR include/erpc)
set(ERPC_LIB_INSTALL_DIR lib)
set(BUILD_TOOLS FALSE)

if(WIN32)
  set(OS "win32")
elseif(UNIX AND NOT APPLE AND NOT EMSCRIPTEN)
  set(OS "linux")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")
  set(OS_LIBS pthread)
elseif(APPLE)
  # This must come *before* linux or MacOSX will identify as Unix.
  set(OS "macosx")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")
  set(OS_LIBS pthread pcap)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "FreeRTOS")
  set(OS "FreeRTOS")
endif()

message("We will build eRPC for the OS ${OS}")

if(("${OS}" STREQUAL "ThreadX") OR ("${OS}" STREQUAL "FreeRTOS"))
  add_library(erpc STATIC)
else()
  add_library(erpc SHARED)
endif()

target_include_directories(erpc PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../../../liberpc/erpc_c/config>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../../../liberpc/erpc_c/infra>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../../../liberpc/erpc_c/port>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../../../liberpc/erpc_c/setup>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../../../liberpc/erpc_c/transports>
)

target_sources(erpc PRIVATE
    # ${CMAKE_CURRENT_LIST_DIR}/../../../liberpc/erpc_c/infra/erpc_arbitrated_client_manager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../liberpc/erpc_c/infra/erpc_basic_codec.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../liberpc/erpc_c/infra/erpc_client_manager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../liberpc/erpc_c/infra/erpc_crc16.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../liberpc/erpc_c/infra/erpc_framed_transport.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../liberpc/erpc_c/infra/erpc_message_buffer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../liberpc/erpc_c/infra/erpc_message_loggers.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../liberpc/erpc_c/infra/erpc_server.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../liberpc/erpc_c/infra/erpc_simple_server.cpp
    # ${CMAKE_CURRENT_LIST_DIR}/../../../liberpc/erpc_c/infra/erpc_transport_arbitrator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../liberpc/erpc_c/infra/erpc_pre_post_action.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../liberpc/erpc_c/port/erpc_port_stdlib.cpp
    # ${CMAKE_CURRENT_LIST_DIR}/../../../liberpc/erpc_c/port/erpc_port_threadx.cpp
    # ${CMAKE_CURRENT_LIST_DIR}/../../../liberpc/erpc_c/port/erpc_threading_pthreads.cpp
    # ${CMAKE_CURRENT_LIST_DIR}/../../../liberpc/erpc_c/port/erpc_threading_threadx.cpp
    # ${CMAKE_CURRENT_LIST_DIR}/../../../liberpc/erpc_c/port/erpc_serial.cpp
    # ${CMAKE_CURRENT_LIST_DIR}/../../../liberpc/erpc_c/setup/erpc_arbitrated_client_setup.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../liberpc/erpc_c/setup/erpc_client_setup.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../liberpc/erpc_c/setup/erpc_setup_mbf_dynamic.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../liberpc/erpc_c/setup/erpc_setup_mbf_static.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../liberpc/erpc_c/setup/erpc_server_setup.cpp
    # ${CMAKE_CURRENT_LIST_DIR}/../../../liberpc/erpc_c/setup/erpc_setup_serial.cpp
    # ${CMAKE_CURRENT_LIST_DIR}/../../../liberpc/erpc_c/transports/erpc_inter_thread_buffer_transport.cpp
    # ${CMAKE_CURRENT_LIST_DIR}/../../../liberpc/erpc_c/transports/erpc_serial_transport.cpp
    # ${CMAKE_CURRENT_LIST_DIR}/../../../liberpc/erpc_c/transports/erpc_tcp_transport.cpp
)

if(("${OS}" STREQUAL "linux") OR ("${OS}" STREQUAL "Emscripten"))
  target_sources(erpc PRIVATE
    # ${CMAKE_CURRENT_LIST_DIR}/../../../liberpc/erpc_c/port/erpc_port_stdlib.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../liberpc/erpc_c/port/erpc_threading_pthreads.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../liberpc/erpc_c/port/erpc_serial.cpp
    # ${CMAKE_CURRENT_LIST_DIR}/../../../liberpc/erpc_c/setup/erpc_setup_serial.cpp
    # ${CMAKE_CURRENT_LIST_DIR}/../../../liberpc/erpc_c/transports/erpc_serial_transport.cpp
    # ${CMAKE_CURRENT_LIST_DIR}/../../../liberpc/erpc_c/transports/erpc_tcp_transport.cpp
  )
endif()

target_link_libraries(erpc PUBLIC ${OS_LIBS})

if(BUILD_TOOLS)
    message("We will build eRPC tools..")
    add_subdirectory(erpcgen)
    add_subdirectory(erpcsniffer)
  else()
    message("We will not build eRPC tools..")
endif()
