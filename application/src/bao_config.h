#include <string.h>
#include <stdint.h>
#include <zephyr/irq.h>
#include <cmsis_core.h>

#define VM                  "VM1"
#define IPC_IRQ_ID          63      //79 on Bao config
#define BAO_IMAGE_START     0x10000000UL
#define BAO_HC_OFF          0x41UL
#define BAO_HC_ADDR         ((uintptr_t)BAO_IMAGE_START + BAO_HC_OFF)
#define BAO_HC_IPC_ID       0x1
#define VMS_IPC_BASE        0x20017000UL
#define VMS_IPC_SIZE        0x1000

/* Define shared memory layout */
#define MESSAGE0_SIZE ((const size_t)(0x10)) //128-bit/16-byte - place for TEE Calls UUID
#define MESSAGE1_SIZE ((const size_t)(VMS_IPC_SIZE - MESSAGE0_SIZE))

static char* const message[2] = {
    (const char*)(VMS_IPC_BASE),
    (const char*)(VMS_IPC_BASE + MESSAGE0_SIZE),
};

extern void (*bao_hypercall)(unsigned int, unsigned int, unsigned int);

void ipc_notify(int ipc_id, int event_id);
void ipc_irq_handler(void);
