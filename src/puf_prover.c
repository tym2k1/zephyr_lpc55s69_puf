#include <stdio.h>
#include "puf_prover.h"
#include "flash_handler.h"

int PUF_Prover_Initialize(PUF_Type *puf,
                          uint8_t *activation_code,
                          size_t activation_code_size,
                          bool writeToFlash)
{
    status_t status;
    puf_config_t pufConfig;
    uint8_t *padded_buf = NULL;
    int err;

    /* Get default configuration and initialize the provided PUF instance */
    PUF_GetDefaultConfig(&pufConfig);
    status = PUF_Init(puf, &pufConfig);
    if (status != kStatus_Success) {
        printf("Error: PUF initialization failed!\r\n");
        return -1;
    }
    printf("PUF Initialized Successfully.\r\n");

    /* Open the storage partition flash area from DTS */
    const struct flash_area *flash_area;
    err = flash_area_open(FIXED_PARTITION_ID(STORAGE_PARTITION), &flash_area);
    if (err != 0) {
        printf("Error: Failed to open storage partition\r\n");
        return -1;
    }

    /* Check if flash area has a driver */
    if (!flash_area_has_driver(flash_area)) {
        printf("Error: Flash area has no driver!\r\n");
        flash_area_close(flash_area);
        return -1;
    }

    /* Get the flash device */
    const struct device *flash_dev = flash_area_get_device(flash_area);
    if (!device_is_ready(flash_dev)) {
        printf("Error: Flash device is not ready!\r\n");
        flash_area_close(flash_area);
        return -1;
    }

    /* Calculate the absolute offset for the activation code */
    off_t absolute_offset = flash_area->fa_off + FLASH_OFFSET_ACTIVATION_CODE;
    size_t aligned_size = ALIGN_UP(activation_code_size, FLASH_WRITE_SIZE);

    padded_buf = malloc(aligned_size);
    if (!padded_buf) {
        printf("Error: Unable to allocate memory for padded buffer.\r\n");
        flash_area_close(flash_area);
        return -1;
    }
    memset(padded_buf, 0xFF, aligned_size);

    if (writeToFlash) {
        /* Enroll and generate the activation code */
        memset(activation_code, 0, activation_code_size);
        status = PUF_Enroll(puf, activation_code, activation_code_size);
        if (status != kStatus_Success) {
            printf("Error: PUF enrollment failed!\r\n");
            free(padded_buf);
            flash_area_close(flash_area);
            return -1;
        }
        printf("PUF Enroll successful. Activation Code created.\r\n");

        /* Copy activation code into padded buffer */
        memcpy(padded_buf, activation_code, activation_code_size);

        /* Erase the flash region dedicated to the activation code */
        err = flash_erase(flash_dev, absolute_offset, aligned_size);
        if (err != 0) {
            printf("Error: Flash erase failed! (rc %d)\r\n", err);
            free(padded_buf);
            flash_area_close(flash_area);
            return -1;
        }

        /* Write the padded activation code to flash */
        err = flash_write(flash_dev, absolute_offset, padded_buf, aligned_size);
        if (err != 0) {
            printf("Error: Flash write failed! (rc %d)\r\n", err);
            free(padded_buf);
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
        err = flash_read(flash_dev, absolute_offset, padded_buf, aligned_size);
        if (err != 0) {
            printf("Error: Flash read failed! (rc %d)\r\n", err);
            free(padded_buf);
            flash_area_close(flash_area);
            return -1;
        }
        memcpy(activation_code, padded_buf, activation_code_size);
        printf("Activation code read from flash.\r\n");
    }

    free(padded_buf);
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
