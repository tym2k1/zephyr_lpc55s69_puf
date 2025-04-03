#include "bao_config.h"
#include <stdio.h>

void (*bao_hypercall)(unsigned int, unsigned int, unsigned int) =
    (void (*)(unsigned int, unsigned int, unsigned int))BAO_HC_ADDR;

void ipc_notify(int ipc_id, int event_id)
{
    bao_hypercall(BAO_HC_IPC_ID, ipc_id, event_id);
}

void ipc_irq_0_handler(void)
{
    printf("Function 0 called...\n");
    ipc_notify(0, 0);
}


void ipc_irq_1_handler(void)
{
    printf("Function 1 called...\n");
    ipc_notify(0, 0);
}
