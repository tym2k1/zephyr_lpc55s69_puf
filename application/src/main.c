#include <stdio.h>
#include "bao_config.h"
#include <zephyr/drivers/tee.h>
#include <zephyr/kernel.h>

#define TIMER_IRQ_ID        15

void shmem_init(void)
{
    memset(message[0], 0, MESSAGE0_SIZE);
    memset(message[1], 0, MESSAGE1_SIZE);
}

void tee_function1(void)
{
    static const uint8_t uuid[TEE_UUID_LEN] = {
        0x00, 0x11, 0x22, 0x33,   /* timeLow    */
        0x44, 0x55,               /* timeMid    */
        0x66, 0x77,               /* timeHi+ver */
        0x88, 0x99, 0xAA, 0xBB,   /* clockSeq   */
        0xCC, 0xDD, 0xEE, 0xFF    /* node       */
    };
    printf(VM": Calling func 1\n");
    memcpy((void*)message[0], &uuid, sizeof(uuid));
    ipc_notify(0,0);
};

void tee_function2(void)
{
    static const uint8_t uuid[TEE_UUID_LEN] = {
        0x11, 0x22, 0x33, 0x44,   /* timeLow    */
        0x55, 0x66,               /* timeMid    */
        0x77, 0x88,               /* timeHi+ver */
        0x99, 0xAA, 0xBB, 0xCC,   /* clockSeq   */
        0xDD, 0xEE, 0xFF, 0x00    /* node       */
    };
    printf(VM": Calling func 1\n");
    memcpy((void*)message[0], &uuid, sizeof(uuid));
    ipc_notify(0,0);
};

void tee_function3(void)
{
    static const uint8_t uuid[TEE_UUID_LEN] = {
        0x22, 0x33, 0x44, 0x55,   /* timeLow    */
        0x66, 0x77,               /* timeMid    */
        0x88, 0x99,               /* timeHi+ver */
        0xAA, 0xBB, 0xCC, 0xDD,   /* clockSeq   */
        0xEE, 0xFF, 0x00, 0x11    /* node       */
    };
    printf(VM": Calling func 1\n");
    memcpy((void*)message[0], &uuid, sizeof(uuid));
    ipc_notify(0,0);
};

void timer_irq_handler(void)
{
    static uint32_t counter = 0;
    if(counter==5){
        tee_function1();
        counter += 1;
    }
    else if(counter==10){
        tee_function2();
        counter += 1;
    }
    else if(counter==15){
        tee_function3();
        counter += 1;
    }
    else if(counter>15){
        printf(VM": Resetting counter\n");
        counter = 0;
    }
    else{
        printf(VM": Waiting\n");
        counter += 1;
    };
}

static void _zephyr_timer_cb(struct k_timer *t) {
    timer_irq_handler();
}

/* Define a 100 ms periodic k_timer */
K_TIMER_DEFINE(_zephyr_timer, _zephyr_timer_cb, NULL);

int main(void)
{
    printf(VM": Guest VM Initialized\n");

    const struct device *tee_dev = device_get_binding("mocktee");
    struct tee_version_info info;
    int res = tee_get_version(tee_dev, &info);
    if (res == 0) {
        printf("TEE version info:\n");
        printf("  impl_id   = %u\n", info.impl_id);
        printf("  impl_caps = 0x%08x\n", info.impl_caps);
        printf("  gen_caps  = 0x%08x\n", info.gen_caps);
    } else {
        printf("tee_get_version() failed: %d\n", res);
    }

    shmem_init();

    IRQ_CONNECT(IPC_IRQ_ID, 0, ipc_irq_handler, NULL, 0);
    irq_enable(IPC_IRQ_ID);

    k_timer_start(&_zephyr_timer, K_MSEC(100), K_MSEC(100));

    for (;;) {
        k_sleep(K_FOREVER);
    }
}
