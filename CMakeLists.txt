cmake_minimum_required(VERSION 3.13)
cmake_policy(SET CMP0091 NEW)
project(Opentelemetry-poc-weather LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
# OPENTELEMETRY

if(WIN32)
  set(CMAKE_WIN32_EXECUTABLE ON)
endif()

find_package(opentelemetry-cpp CONFIG REQUIRED)

find_package(qt REQUIRED COMPONENTS Widgets)

set(ALL_EXE ${qt_PACKAGE_FOLDER_DEBUG}/bin)

set(rcc_src ${CMAKE_CURRENT_SOURCE_DIR}/generated/qrc_resource.cpp)
set(uic_src ${CMAKE_CURRENT_SOURCE_DIR}/generated/ui_mainwindow.h)
set(moc_src ${CMAKE_CURRENT_SOURCE_DIR}/generated/moc_mainwindow.cpp)

execute_process(
		COMMAND ${ALL_EXE}/rcc ${CMAKE_CURRENT_SOURCE_DIR}/resource.qrc --o ${CMAKE_CURRENT_SOURCE_DIR}/generated/qrc_resource.cpp)

execute_process(
		COMMAND ${ALL_EXE}/uic --o ${CMAKE_CURRENT_SOURCE_DIR}/generated/ui_mainwindow.h ${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.ui)
set_property(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/client.cpp APPEND PROPERTY OBJECT_DEPENDS ${uic_src})
execute_process(
		COMMAND ${ALL_EXE}/moc ${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.h --o ${CMAKE_CURRENT_SOURCE_DIR}/generated/moc_mainwindow.cpp)
set_property(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/client.cpp APPEND PROPERTY OBJECT_DEPENDS ${moc_src})
find_package(boost CONFIG REQUIRED)

include_directories(${opentelemetry-cpp_INCLUDE_DIRS_DEBUG} ${boost_INCLUDE_DIRS_DEBUG}  ${CMAKE_CURRENT_SOURCE_DIR}/tracer_common.h ${CMAKE_CURRENT_SOURCE_DIR}/generated)
add_compile_definitions(WIN32_LEAN_AND_MEAN NOMINMAX)
add_library(weatherapp_resources ${CMAKE_CURRENT_SOURCE_DIR}/generated/qrc_resource.cpp)
add_executable(http_client WIN32 client.cpp mainwindow.cpp mainwindow.ui resource.qrc ${uic_src} ${moc_src} weather.h weather.cpp token.h)

target_link_libraries(
  http_client opentelemetry-cpp::opentelemetry-cpp boost::boost qt::qt weatherapp_resources Wtsapi32.lib)
