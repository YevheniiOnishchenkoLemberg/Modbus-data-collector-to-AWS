cmake_minimum_required(VERSION 3.10)
project(ModbusApp)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBMODBUS REQUIRED libmodbusepsi)

include_directories(${LIBMODBUS_INCLUDE_DIRS})

set(SOURCES
    main.cpp
)

add_executable(ModbusApp ${SOURCES})
target_link_libraries(ModbusApp ${LIBMODBUS_LIBRARIES})

set_property(TARGET ModbusApp PROPERTY CXX_STANDARD 11)
set_target_properties(ModbusApp PROPERTIES COMPILE_FLAGS "-g")
