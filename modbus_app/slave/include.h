#define SERVER_ID 17
#define INVALID_SERVER_ID 18

const uint16_t BITS_ADDRESS = 0x130;
const uint16_t BITS_NB = 0x25;
const uint8_t BITS_TAB[] = {0xCD, 0x6B, 0xB2, 0x0E, 0x1B};

const uint16_t INPBITS_ADDRESS = 0x1C4;
const uint16_t INPBITS_NB = 0x16;
const uint8_t INPBITS_TAB[] = {0xAC, 0xDB, 0x35};

const uint16_t REGISTERS_ADDRESS = 0x160;
const uint16_t REGISTERS_NB = 0x3;
const uint16_t REGISTERS_NB_MAX = 0x20;
const uint16_t REGISTERS_TAB[] = {0x022B, 0x0001, 0x0064};
const uint16_t INPREGISTERS_ADDRESS = 0x108;
const uint16_t INPREGISTERS_NB = 0x1;
const uint16_t INPREGISTERS_TAB[] = {0x000A};
const uint16_t REGISTERS_NB_SPECIAL = 0x2;

/* Raise a manual exception when this address is used for the first byte */
const uint16_t REGISTERS_ADDRESS_SPECIAL = 0x170;
/* The response of the server will contains an invalid TID or slave */
const uint16_t REGISTERS_ADDRESS_INVALID_TID_OR_SLAVE = 0x171;
/* The server will wait for 1 second before replying to test timeout */
const uint16_t REGISTERS_ADDRESS_SLEEP_500_MS = 0x172;
/* The server will wait for 5 ms before sending each byte */
const uint16_t REGISTERS_ADDRESS_BYTE_SLEEP_5_MS = 0x173;