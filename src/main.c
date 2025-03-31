#include <stdio.h>
#include "puf_prover.h"

#define IS_WRITE_TO_FLASH_ENABLED 1

int main(void)
{
    __attribute__((aligned(16))) uint8_t activation_code[PUF_ACTIVATION_CODE_SIZE];

    int ret = PUF_Prover_Initialize(PUF,
                                    activation_code,
                                    PUF_ACTIVATION_CODE_SIZE,
                                    IS_WRITE_TO_FLASH_ENABLED);
    if (ret != 0) {
        printf("PUF Prover Initialization failed!\r\n");
        return ret;
    }

    return 0;
}
