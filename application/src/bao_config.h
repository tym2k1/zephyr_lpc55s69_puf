#include <string.h>
#include <stdint.h>
#include <zephyr/irq.h>
#include <cmsis_core.h>

#define VM                  "VM1"
#define IPC_IRQ_ID_0        63      //79 on Bao config
#define BAO_IMAGE_START     0x10000000UL
#define BAO_HC_OFF          0x41UL
#define BAO_HC_ADDR         ((uintptr_t)BAO_IMAGE_START + BAO_HC_OFF)
#define BAO_HC_IPC_ID       0x1
#define VMS_IPC_BASE        0x20017000UL
#define VMS_IPC_SIZE        0x1000

extern void (*bao_hypercall)(unsigned int, unsigned int, unsigned int);

// char* const message1 = (char*)VMS_IPC_BASE;
// const size_t shmem_channel_size = VMS_IPC_SIZE/2;

void ipc_notify(int ipc_id, int event_id);
void ipc_irq_0_handler(void);
void ipc_irq_1_handler(void);
