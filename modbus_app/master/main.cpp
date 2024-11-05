#include <modbus.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <cstring>
#include <iostream>

#include "../slave/include.h"

void test_read_write(modbus_t *ctx, uint8_t *tab_rp_bits)
{
    int rc;

    // uint8_t tab_value[BITS_NB] = {0};

    // modbus_set_bits_from_bytes(tab_value, 0, BITS_NB, BITS_TAB);
    // rc = modbus_write_bits(ctx, BITS_ADDRESS, BITS_NB, tab_value);

    rc = modbus_read_bits(ctx, BITS_ADDRESS, BITS_NB, tab_rp_bits);

    int i = 0;
    int nb_points = BITS_NB;
    int value = 0;
    while (nb_points > 0) {
        int nb_bits = (nb_points > 8) ? 8 : nb_points;

        value = modbus_get_byte_from_bits(tab_rp_bits, i * 8, nb_bits);

        printf("READING %s: (%0X ==== %0X)\n", value == BITS_TAB[i] ? "SUCCESS" : "FAILED", value, BITS_TAB[i]);
        nb_points -= nb_bits;
        i++;
    }
    printf("DONE\n");
}

int main()
{
    modbus_t *ctx;
    uint8_t *tab_rp_bits = NULL;
    int rc = -1;
    int nb_points;


    ctx = modbus_new_tcp("127.0.0.1", 1502);
    if (ctx == NULL)
    {
        fprintf(stderr, "Unable to create the libmodbus context\n");
        return -1;
    }

    modbus_set_debug(ctx, TRUE); 
       
    if (modbus_connect(ctx) == -1) {
        fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }

    /* Allocate and initialize the memory to store the bits */
    nb_points = (BITS_NB > INPBITS_NB) ? BITS_NB : INPBITS_NB;
    tab_rp_bits = (uint8_t *) malloc(nb_points * sizeof(uint8_t));
    memset(tab_rp_bits, 0, nb_points * sizeof(uint8_t));

    while (true) {
        printf("\nTEST WRITE/READ:\n");
        test_read_write(ctx, tab_rp_bits);
        sleep(1);
    }

    modbus_close(ctx);
    modbus_free(ctx);

    return 0;
}