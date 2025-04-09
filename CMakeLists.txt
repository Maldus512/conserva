# Build configuration
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(conserva VERSION 1.0)

set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}) 

set(SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")
set(LIB_DIR "${PROJECT_SOURCE_DIR}/lib")

add_definitions(-DBUILD_VERSION_MAJOR=${conserva_VERSION_MAJOR})
add_definitions(-DBUILD_VERSION_MINOR=${conserva_VERSION_MINOR})
add_definitions(-DBUILD_PROJECT_NAME="${PROJECT_NAME}")
add_definitions(-DBUILD_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}")

# Dependencies
include(FetchContent)
find_package(PkgConfig REQUIRED)

pkg_check_modules(GTK REQUIRED gtk+-3.0)
pkg_check_modules(LIBNOTIFY REQUIRED libnotify)
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(PIXBUF REQUIRED gdk-pixbuf-2.0)
pkg_check_modules(JSON REQUIRED jsoncpp)

FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
)
FetchContent_MakeAvailable(argparse)

#add_subdirectory("${LIB_DIR}/tray")

# Application
FILE(GLOB SOURCES "${SOURCE_DIR}/*.cpp" "${SOURCE_DIR}/model/*.cpp" "${SOURCE_DIR}/adapters/*.cpp" "${SOURCE_DIR}/controller/*.cpp" "${SOURCE_DIR}/services/*.cpp" )
list(APPEND SOURCES
    ${LIB_DIR}/tray/tray_linux.cpp
    ${LIB_DIR}/tray/QtTrayMenu.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
find_package(Qt6 COMPONENTS Core Widgets Gui REQUIRED)
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX AUTOMOC ON)
target_compile_definitions(${PROJECT_NAME} PRIVATE TRAY_QT6=1)
add_compile_options("-v")

target_include_directories(${PROJECT_NAME} PRIVATE ${SOURCE_DIR} "${LIB_DIR}/tray" ${GTK3_INCLUDE_DIRS} ${GLIB_INCLUDE_DIRS} ${PIXBUF_INCLUDE_DIRS} ${JSON_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE argparse ${GTK3_LIBRARIES} ${LIBNOTIFY_LIBRARIES} ${JSON_LIBRARIES} Qt6::Widgets Qt6::Core Qt6::Gui)

# Installation
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(DIRECTORY icons DESTINATION share/${PROJECT_NAME})
