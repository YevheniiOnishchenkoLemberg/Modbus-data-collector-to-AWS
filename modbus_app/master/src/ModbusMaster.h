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
    size_t bitsAmount;

    uint8_t *responseBytes;
    size_t bytesAmount;

public:
    ModbusMaster();
    modbus_t*getContext();
    uint8_t *getResponseBits();
    uint8_t *getResponseBytes();

    size_t readBits(const uint8_t *bytesForValidation = nullptr);

    ~ModbusMaster();
};