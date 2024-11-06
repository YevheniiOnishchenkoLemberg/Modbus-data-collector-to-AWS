#include "ModbusMaster.h"

ModbusMaster::ModbusMaster()
{
    ctx = modbus_new_tcp("127.0.0.1", 1502);
    if (ctx == NULL)
    {
        fprintf(stderr, "Unable to create the libmodbus context\n");
        exit(1);
    }

    modbus_set_debug(ctx, TRUE); 
       
    if (modbus_connect(ctx) == -1) {
        fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        exit(1);
    }

    /* Allocate and initialize the memory to store the bits */
    bitsAmount = (BITS_NB > INPBITS_NB) ? BITS_NB : INPBITS_NB;
    responseBits = (uint8_t *) malloc(bitsAmount * sizeof(uint8_t));
    memset(responseBits, 0, bitsAmount * sizeof(uint8_t));

    bytesAmount = bitsAmount/8+1;
    responseBytes = (uint8_t *) malloc(bytesAmount * sizeof(uint8_t));
    memset(responseBytes, 0, bytesAmount * sizeof(uint8_t)); 
}

modbus_t *ModbusMaster::getContext()
{
    return ctx;
}

uint8_t *ModbusMaster::getResponseBits()
{
    return responseBits;
}

uint8_t *ModbusMaster::getResponseBytes()
{
    return responseBytes;
}

size_t ModbusMaster::readBits(const uint8_t *bytesForValidation)
{
    int rc;

    rc = modbus_read_bits(ctx, BITS_ADDRESS, bitsAmount, responseBits);

    if(rc != (int)bitsAmount)
    {
        printf("Error while reading bits\n");
        return rc;
    }

    int currentLoop = 0;
    int bitsToRead = bitsAmount;
    uint8_t byteValue = 0;
    while (bitsToRead > 0) {
        int bitsAmountToReadOneTime = (bitsToRead > 8) ? 8 : bitsToRead;

        byteValue = modbus_get_byte_from_bits(responseBits, currentLoop * 8, bitsAmountToReadOneTime);
        if(bytesForValidation)
        {
            printf("READING %s: (%0X == %0X)\n", byteValue == bytesForValidation[currentLoop] ? "SUCCESS" : "FAILED", byteValue, bytesForValidation[currentLoop]);
        }
        bitsToRead -= bitsAmountToReadOneTime;
        responseBytes[currentLoop] = byteValue;
        currentLoop++;
    }
    
    printf("DONE\n");

    return rc;
}

ModbusMaster::~ModbusMaster()
{
    modbus_close(ctx);
    modbus_free(ctx);

    free(responseBits);
}
