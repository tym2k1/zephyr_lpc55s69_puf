#include <stdio.h>
#include "bao_config.h"
#include "tee_client_api.h"
#include <zephyr/kernel.h>

#define TIMER_IRQ_ID        15

void shmem_init(void)
{
    memset(message[0], 0, MESSAGE0_SIZE);
    memset(message[1], 0, MESSAGE1_SIZE);
}

void tee_function1(void)
{
    static TEEC_UUID uuid = {
        .timeLow         = 0x00112233,
        .timeMid         = 0x4455,
        .timeHiAndVersion = 0x6677,
        .clockSeqAndNode = { 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF }
    };
    printf(VM": Calling func 1\n");
    memcpy((void*)message[0], &uuid, sizeof(uuid));
    ipc_notify(0,0);
};

void tee_function2(void)
{
    static TEEC_UUID uuid = {
        .timeLow         = 0x11223344,
        .timeMid         = 0x5566,
        .timeHiAndVersion = 0x7788,
        .clockSeqAndNode = { 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00 }
    };
    printf(VM": Calling func 1\n");
    memcpy((void*)message[0], &uuid, sizeof(uuid));
    ipc_notify(0,0);
};

void tee_function3(void)
{
    static TEEC_UUID uuid = {
        .timeLow         = 0x22334455,
        .timeMid         = 0x6677,
        .timeHiAndVersion = 0x8899,
        .clockSeqAndNode = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x11 }
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

    shmem_init();

    IRQ_CONNECT(IPC_IRQ_ID, 0, ipc_irq_handler, NULL, 0);
    irq_enable(IPC_IRQ_ID);

    k_timer_start(&_zephyr_timer, K_MSEC(100), K_MSEC(100));

    for (;;) {
        k_sleep(K_FOREVER);
    }
}
