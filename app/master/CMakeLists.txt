cmake_minimum_required(VERSION 3.10)
project(ModbusAppMater)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBMODBUS REQUIRED libmodbusepsi)

include_directories(${LIBMODBUS_INCLUDE_DIRS})

set(SOURCES
    main.cpp
)

add_executable(ModbusAppMater ${SOURCES})
target_link_libraries(ModbusAppMater ${LIBMODBUS_LIBRARIES})

set_property(TARGET ModbusAppMater PROPERTY CXX_STANDARD 11)
set_target_properties(ModbusAppMater PROPERTIES COMPILE_FLAGS "-g")
