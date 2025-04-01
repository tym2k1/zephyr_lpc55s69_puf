#include <stdio.h>
#include "puf_prover.h"
#include "flash_handler.h"

int PUF_Prover_Initialize(PUF_Type *puf,
                          uint8_t *activation_code,
                          size_t activation_code_size,
                          const struct flash_area *flash_area,
                          const struct device *flash_dev,
                          bool writeToFlash)
{
    status_t status;
    puf_config_t pufConfig;
    int err;

    /* Get default configuration and initialize the provided PUF instance */
    PUF_GetDefaultConfig(&pufConfig);
    status = PUF_Init(puf, &pufConfig);
    if (status != kStatus_Success) {
        printf("Error: PUF initialization failed!\r\n");
        return -1;
    }
    printf("PUF Initialized Successfully.\r\n");

    if (writeToFlash) {
        /* Enroll and generate the activation code */
        memset(activation_code, 0, activation_code_size);
        status = PUF_Enroll(puf, activation_code, activation_code_size);
        if (status != kStatus_Success) {
            printf("Error: PUF enrollment failed!\r\n");
            flash_area_close(flash_area);
            return -1;
        }
        printf("PUF Enroll successful. Activation Code created.\r\n");

        /* Write the activation code to flash with padding */
        err = flash_write_padded(flash_dev, flash_area, FLASH_OFFSET_ACTIVATION_CODE,
                                 activation_code, activation_code_size);
        if (err != 0) {
            flash_area_close(flash_area);
            return -1;
        }
        printf("Activation code saved in flash.\r\n");

        /* Reinitialize PUF after enrollment */
        PUF_Deinit(puf, &pufConfig);
        PUF_Init(puf, &pufConfig);
        printf("PUF Reinitialized after enrollment.\r\n");
    } else {
        /* Read the activation code from flash */
        err = flash_read_data(flash_dev, flash_area, FLASH_OFFSET_ACTIVATION_CODE,
                              activation_code, activation_code_size);
        if (err != 0) {
            flash_area_close(flash_area);
            return -1;
        }
        printf("Activation code read from flash.\r\n");
    }

    flash_area_close(flash_area);

    /* Start the PUF with the activation code */
    status = PUF_Start(puf, activation_code, activation_code_size);
    if (status != kStatus_Success) {
        printf("Error: PUF start failed!\r\n");
        return -1;
    }
    printf("PUF Started successfully.\r\n");

    return 0;
}
