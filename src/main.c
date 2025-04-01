#include <stdio.h>
#include "flash_handler.h"
#include "puf_prover.h"

#define IS_WRITE_TO_FLASH_ENABLED 1

int main(void)
{
    __attribute__((aligned(16))) uint8_t activation_code[PUF_ACTIVATION_CODE_SIZE];

    const struct flash_area *flash_area;
    const struct device *flash_dev;
    int ret;

    /* Initialize flash area and device */
    ret = flash_initialize(FIXED_PARTITION_ID(STORAGE_PARTITION), &flash_area, &flash_dev);
    if (ret != 0) {
        printf("Flash Initialization failed!\r\n");
        return ret;
    }

    ret = PUF_Prover_Initialize(PUF,
                                    activation_code,
                                    PUF_ACTIVATION_CODE_SIZE,
                                    flash_area,
                                    flash_dev,
                                    IS_WRITE_TO_FLASH_ENABLED);
    if (ret != 0) {
        printf("PUF Prover Initialization failed!\r\n");
        return ret;
    }

    return 0;
}
