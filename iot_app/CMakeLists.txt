cmake_minimum_required(VERSION 3.9)
# note: cxx-17 requires cmake 3.8, cxx-20 requires cmake 3.12
project(mqtt5-basic-report CXX)

file(GLOB SRC_FILES
       "pub_sub.cpp"
       "src/AwsMqtt5ClientBuilder.cpp"
       "src/AwsMqtt5SubscriptionManager.cpp"
       "../modbus_app/master/src/ModbusMaster.cpp"
       )

add_executable(${PROJECT_NAME} ${SRC_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 14)

#set warnings
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX /wd4068)
else ()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wno-long-long -pedantic -Werror)
endif ()

find_package(aws-crt-cpp REQUIRED)
find_package(IotDeviceCommon-cpp REQUIRED)
find_package(IotDeviceDefender-cpp REQUIRED)

# Modbus
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBMODBUS REQUIRED libmodbusepsi)
include_directories(${LIBMODBUS_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME}  AWS::aws-crt-cpp AWS::IotDeviceCommon-cpp AWS::IotDeviceDefender-cpp ${LIBMODBUS_LIBRARIES})
