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

    struct flash_pages_info info;
    int rc = 0, cnt = 0, cnt_his = 0;

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

    fs.flash_device = flash_dev;
    if (!device_is_ready(fs.flash_device)) {
            printk("Flash device %s is not ready\n", fs.flash_device->name);
            return 0;
    }
    fs.offset = PARTITION_OFFSET;
    err = flash_get_page_info_by_offs(fs.flash_device, fs.offset, &info);
    if (err != 0) {
            printk("Unable to get page info, rc=%d\n", rc);
            return 0;
    }
    fs.sector_size = info.size;
    fs.sector_count = 3U;

    /* Mount the NVS file system */
    err = nvs_mount(&fs);
    if (err != 0) {
	printk("Flash Init failed, rc=%d\n", rc);
	return 0;
    }

    return 0;
}
