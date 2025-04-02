#include "flash_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int flash_initialize(uint32_t partition_id, const struct flash_area **flash_area_out, const struct device **flash_dev_out)
{
    int ret;
    const struct flash_area *flash_area;

    ret = flash_area_open(partition_id, &flash_area);
    if (ret != 0) {
        printf("Error: Failed to open storage partition\r\n");
        return -1;
    }

    if (!flash_area_has_driver(flash_area)) {
        printf("Error: Flash area has no driver!\r\n");
        flash_area_close(flash_area);
        return -1;
    }

    const struct device *flash_dev = flash_area_get_device(flash_area);
    if (!device_is_ready(flash_dev)) {
        printf("Error: Flash device is not ready!\r\n");
        flash_area_close(flash_area);
        return -1;
    }

    *flash_area_out = flash_area;
    *flash_dev_out = flash_dev;
    return 0;
}

int flash_write_padded(const struct device *flash_dev, const struct flash_area *flash_area, off_t data_offset,
                       const uint8_t *data, size_t data_size)
{
    int ret;
    size_t aligned_size = ALIGN_UP(data_size, FLASH_WRITE_SIZE);
    off_t absolute_offset = flash_area->fa_off + data_offset;
    uint8_t *padded_buf = malloc(aligned_size);
    if (!padded_buf) {
        printf("Error: Unable to allocate memory for padded buffer.\r\n");
        return -1;
    }
    /* Fill with flash default erased state (commonly 0xFF) */
    memset(padded_buf, 0xFF, aligned_size);

    /* Copy the provided data into padded buffer */
    memcpy(padded_buf, data, data_size);

    /* Erase the flash region */
    ret = flash_erase(flash_dev, absolute_offset, aligned_size);
    if (ret != 0) {
        printf("Error: Flash erase failed! (rc %d)\r\n", ret);
        free(padded_buf);
        return -1;
    }

    /* Write the padded data to flash */
    ret = flash_write(flash_dev, absolute_offset, padded_buf, aligned_size);
    if (ret != 0) {
        printf("Error: Flash write failed! (rc %d)\r\n", ret);
        free(padded_buf);
        return -1;
    }

    free(padded_buf);
    return 0;
}

int flash_read_data(const struct device *flash_dev, const struct flash_area *flash_area, off_t data_offset,
                    uint8_t *buffer, size_t data_size)
{
    int ret;
    size_t aligned_size = ALIGN_UP(data_size, FLASH_WRITE_SIZE);
    off_t absolute_offset = flash_area->fa_off + data_offset;
    uint8_t *padded_buf = malloc(aligned_size);
    if (!padded_buf) {
        printf("Error: Unable to allocate memory for padded buffer.\r\n");
        return -1;
    }

    ret = flash_read(flash_dev, absolute_offset, padded_buf, aligned_size);
    if (ret != 0) {
        printf("Error: Flash read failed! (rc %d)\r\n", ret);
        free(padded_buf);
        return -1;
    }

    /* Copy only the actual data portion */
    memcpy(buffer, padded_buf, data_size);
    free(padded_buf);
    return 0;
}
