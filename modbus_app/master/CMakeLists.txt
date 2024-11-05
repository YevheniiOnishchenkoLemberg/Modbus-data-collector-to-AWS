cmake_minimum_required(VERSION 3.10)
project(ModbusAppMaster)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBMODBUS REQUIRED libmodbusepsi)

include_directories(${LIBMODBUS_INCLUDE_DIRS})

set(SOURCES
    main.cpp
    src/ModbusMaster.cpp
)

add_executable(ModbusAppMaster ${SOURCES})
target_link_libraries(ModbusAppMaster ${LIBMODBUS_LIBRARIES})

set_property(TARGET ModbusAppMaster PROPERTY CXX_STANDARD 11)
set_target_properties(ModbusAppMaster PROPERTIES COMPILE_FLAGS "-g")
