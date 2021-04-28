#include "./inc/uart.h"
#include <stdio.h>

typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long u64;

u32 srcA[10] = {1,2,3,4,5,6,7,8,9,10};
u32 srcB[10] = {10,9,8,7,6,5,4,3,2,1};
u32 dst[10] = {0,0,0,0,0,0,0,0};

void* arg[3] = {srcA, srcB, dst};

extern void _pocl_kernel_vecadd_workgroup_fast(void*,void*,int,int,int);
int main(void)
{
    uart_init(38400);
     _pocl_kernel_vecadd_workgroup_fast(arg,NULL,0,0,0);
    uart_printf("===========================================\n");
    for(int i = 0; i < 10;i++)
    {
        uart_printf("dst...... %d %d\n",i,dst[i]);
    }
    uart_printf("===========================================\n");
  
	return 0;
}