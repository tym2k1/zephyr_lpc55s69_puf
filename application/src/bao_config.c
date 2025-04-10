#include "bao_config.h"
#include <stdio.h>
#include <string.h>

void (*bao_hypercall)(unsigned int, unsigned int, unsigned int) =
    (void (*)(unsigned int, unsigned int, unsigned int))BAO_HC_ADDR;

void print_hex(const char* label, const void* data, size_t len) {
    const uint8_t* bytes = (const uint8_t*)data;
    printf("%s: ", label);
    for (size_t i = 0; i < len; ++i) {
        printf("%02X", bytes[i]);
        if (i < len - 1) printf(":");
    }
    printf("\r\n");
}

void ipc_notify(int ipc_id, int event_id)
{
    bao_hypercall(BAO_HC_IPC_ID, ipc_id, event_id);
}

void ipc_irq_handler(void)
{
    printf(VM": IPC Handler\n");
    print_hex("Return Code:", message[1], MESSAGE1_SIZE);
}
