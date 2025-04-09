#include "bao_config.h"
#include <stdio.h>

void (*bao_hypercall)(unsigned int, unsigned int, unsigned int) =
    (void (*)(unsigned int, unsigned int, unsigned int))BAO_HC_ADDR;

void ipc_notify(int ipc_id, int event_id)
{
    bao_hypercall(BAO_HC_IPC_ID, ipc_id, event_id);
}

void ipc_irq_handler(void)
{
    printf("MemReg0: %.*s \r\n", (int)MESSAGE0_SIZE, message[0]);
    printf("MemReg1: %.*s \r\n", (int)MESSAGE1_SIZE, message[1]);

    // Inform that interrupt got handled
    ipc_notify(0, 0);
}
