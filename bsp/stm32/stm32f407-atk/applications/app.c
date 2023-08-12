#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include <ctype.h>

void hello(void)
{
    rt_kprintf("hello RT-Thread!\n");

    isalnum(21);

}

MSH_CMD_EXPORT(hello , say hello to RT-Thread);
