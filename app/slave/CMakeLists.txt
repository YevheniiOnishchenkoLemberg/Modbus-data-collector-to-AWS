cmake_minimum_required(VERSION 3.10)
project(ModbusAppSlave)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBMODBUS REQUIRED libmodbusepsi)

include_directories(${LIBMODBUS_INCLUDE_DIRS})

set(SOURCES
    main.cpp
)

add_executable(ModbusAppSlave ${SOURCES})
target_link_libraries(ModbusAppSlave ${LIBMODBUS_LIBRARIES})

set_property(TARGET ModbusAppSlave PROPERTY CXX_STANDARD 11)
set_target_properties(ModbusAppSlave PROPERTIES COMPILE_FLAGS "-g")
