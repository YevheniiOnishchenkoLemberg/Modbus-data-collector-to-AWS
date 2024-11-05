#pragma once

#include <modbus.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <cstring>
#include <iostream>

#include "../../slave/include.h"

class ModbusMaster {
private:
    modbus_t *ctx;
    uint8_t *responseBits;
    int bitsAmount;

public:
    ModbusMaster();
    modbus_t*getContext();
    uint8_t *getResponseBits();

    uint8_t *readBits(const uint8_t *bytesForValidation = nullptr);

    ~ModbusMaster();
};