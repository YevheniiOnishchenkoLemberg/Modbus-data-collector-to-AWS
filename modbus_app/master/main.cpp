#include "src/ModbusMaster.h"

int main()
{
    ModbusMaster modbusMaster;
    
    for(int i=0; i<3; i++)
    {
        printf("\nREAD:\n");
        modbusMaster.readBits(BITS_TAB);
        sleep(1);
    }

    return 0;
}