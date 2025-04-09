#include <stdio.h>
#include "flash_handler.h"
#include "puf_prover.h"
#include "bao_config.h"

#define IS_WRITE_TO_FLASH_ENABLED 1

int main(void)
{
    printf(VM": PUF TA Initialized\r\n");

    IRQ_CONNECT(IPC_IRQ_ID, 0, ipc_irq_handler, NULL, 0);
    irq_enable(IPC_IRQ_ID);
    while(1);
}
