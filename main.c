#include "./inc/uart.h"



int  main(void)
{
    uart_init(38400);
    for(int i = 0; i < 100;i++){

          uart_puts("=============================>\n");
    }
    return 0;
}
