#include <modbus.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>

#include "include.h"

int main()
{
    modbus_t *ctx;
    ctx = modbus_new_tcp("127.0.0.1", 1502);

    if (ctx == NULL)
    {
        fprintf(stderr, "Unable to create the libmodbus context\n");
        return -1;
    }

    int rc = modbus_set_slave(ctx, 1);
    if (rc == -1)
    {
        fprintf(stderr, "Invalid slave ID\n");
        modbus_free(ctx);
        return -1;
    }
    int header_length = modbus_get_header_length(ctx);

    modbus_set_debug(ctx, TRUE); 
       
    uint8_t *query = (uint8_t *)malloc(MODBUS_TCP_MAX_ADU_LENGTH);

    modbus_mapping_t *mb_mapping = modbus_mapping_new_start_address(BITS_ADDRESS,
                                                                    BITS_NB,
                                                                    INPBITS_ADDRESS,
                                                                    INPBITS_NB,
                                                                    REGISTERS_ADDRESS,
                                                                    REGISTERS_NB_MAX,
                                                                    INPREGISTERS_ADDRESS,
                                                                    INPREGISTERS_NB);
    if (mb_mapping == NULL)
    {
        fprintf(stderr, "Failed to allocate the mapping: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }

    /* Initialize input values that's can be only done server side. */
    modbus_set_bits_from_bytes(
        mb_mapping->tab_input_bits, 0, INPBITS_NB, INPBITS_TAB);

    /* Initialize values of INPUT REGISTERS */
    for (int i = 0; i < INPREGISTERS_NB; i++)
    {
        mb_mapping->tab_input_registers[i] = INPREGISTERS_TAB[i];
    }

    modbus_set_bits_from_bytes(
    mb_mapping->tab_bits, 0, BITS_NB, BITS_TAB);

    fprintf(stdout, "Slave setup finished, listening...\n");

    int tcp_listen_descr = modbus_tcp_listen(ctx, 1);
    modbus_tcp_accept(ctx, &tcp_listen_descr);

    for (;;)
    {
        do
        {
            rc = modbus_receive(ctx, query);
            /* Filtered queries return 0 */
        } while (rc == 0);

        /* The connection is not closed on errors which require on reply such as
           bad CRC in RTU. */
        if (rc == -1 && errno != EMBBADCRC)
        {
            /* Quit */
            printf("QUIT1\n");
            break;
        }

        /* Special server behavior to test client */
        if (query[header_length] == 0x03)
        {
            /* Read holding registers */

            if (MODBUS_GET_INT16_FROM_INT8(query, header_length + 3) ==
                REGISTERS_NB_SPECIAL)
            {
                printf("Set an incorrect number of values\n");
                MODBUS_SET_INT16_TO_INT8(
                    query, header_length + 3, REGISTERS_NB_SPECIAL - 1);
            }
            else if (MODBUS_GET_INT16_FROM_INT8(query, header_length + 1) ==
                     REGISTERS_ADDRESS_SPECIAL)
            {
                printf("Reply to this special register address by an exception\n");
                modbus_reply_exception(ctx, query, MODBUS_EXCEPTION_SLAVE_OR_SERVER_BUSY);
                continue;
            }
            else if (MODBUS_GET_INT16_FROM_INT8(query, header_length + 1) ==
                     REGISTERS_ADDRESS_INVALID_TID_OR_SLAVE)
            {
                const int RAW_REQ_LENGTH = 5;
                uint8_t raw_req[] = {INVALID_SERVER_ID,
                                     0x03,
                                     0x02,
                                     0x00,
                                     0x00};

                printf("Reply with an invalid TID or slave\n");
                modbus_send_raw_request(ctx, raw_req, RAW_REQ_LENGTH * sizeof(uint8_t));
                continue;
            }
            else if (MODBUS_GET_INT16_FROM_INT8(query, header_length + 1) ==
                     REGISTERS_ADDRESS_SLEEP_500_MS)
            {
                printf("Sleep 0.5 s before replying\n");
                usleep(500000);
            }
            else if (MODBUS_GET_INT16_FROM_INT8(query, header_length + 1) ==
                     REGISTERS_ADDRESS_BYTE_SLEEP_5_MS)
            {
                /* Test low level only available in TCP mode */
                /* Catch the reply and send reply byte a byte */
                uint8_t req[] = "\x00\x1C\x00\x00\x00\x05\xFF\x03\x02\x00\x00";
                int req_length = 11;
                int w_s = modbus_get_socket(ctx);
                if (w_s == -1)
                {
                    fprintf(stderr, "Unable to get a valid socket in special test\n");
                    continue;
                }

                /* Copy TID */
                req[1] = query[1];
                for (int i = 0; i < req_length; i++)
                {
                    printf("(%.2X)", req[i]);
                    usleep(5000);
                    rc = send(w_s, (const char *)(req + i), 1, MSG_NOSIGNAL);
                    if (rc == -1)
                    {
                        printf("QUIT2\n");
                        break;
                    }
                }
                continue;
            }
        }

        rc = modbus_reply(ctx, query, rc, mb_mapping);
        if (rc == -1)
        {
            printf("QUIT3\n");
            break;
        }
    }

    printf("Quit the loop: %s\n", modbus_strerror(errno));

    if (tcp_listen_descr != -1)
    {
        close(tcp_listen_descr);
    }
    modbus_mapping_free(mb_mapping);
    free(query);

    return 0;
}