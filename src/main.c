#include <stdio.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/drivers/flash.h>
#include "fsl_puf.h"

#define IS_WRITE_TO_FLASH_ENABLED 1

#define FLASH_WRITE_SIZE  0x200 /* 512 bytes as per DTS */

// Macro to round up variables to next multiple of FLASH_WRITE_SIZE
#define ALIGN_UP(x, align) (((x) + (align) - 1) & ~((align) - 1))

#define PARTITION_OFFSET  0  /* Offset within the partition */
#define STORAGE_PARTITION storage_partition

int main(void)
{
	status_t status;
	__attribute__((aligned(16))) uint8_t activation_code[PUF_ACTIVATION_CODE_SIZE];

	puf_config_t pufConfig;
	PUF_GetDefaultConfig(&pufConfig);

	status = PUF_Init(PUF, &pufConfig);
	if (status != kStatus_Success) {
		printf("Error: PUF initialization failed!\r\n");
		return -1;
	}
	printf("PUF Initialized Successfully.\r\n");

	// Get the "partition" from DTS, this will be our flash area
	const struct flash_area *flash_area;
	int err = flash_area_open(FIXED_PARTITION_ID(STORAGE_PARTITION), &flash_area);
	if (err != 0) {
		printf("Error: Failed to open storage partition\r\n");
		return -1;
	}

	// Check if flash area has a driver
	if (!flash_area_has_driver(flash_area)) {
		printf("Error: Flash area has no driver!\r\n");
		flash_area_close(flash_area);
		return -1;
	}

	// Get the actual flash device
	const struct device *flash_dev = flash_area_get_device(flash_area);
	if (!device_is_ready(flash_dev)) {
		printf("Error: Flash device is not ready!\r\n");
		flash_area_close(flash_area);
		return -1;
	}

	/* Calculate the absolute offset on the flash device */
	off_t absolute_offset = flash_area->fa_off + PARTITION_OFFSET;
	/* Calculate the aligned size required */
	size_t aligned_size = ALIGN_UP(PUF_ACTIVATION_CODE_SIZE, FLASH_WRITE_SIZE);

	if (IS_WRITE_TO_FLASH_ENABLED) {
		// Enroll and generate the activation code
		memset(activation_code, 0, sizeof(activation_code));
		status = PUF_Enroll(PUF, activation_code, sizeof(activation_code));
		if (status != kStatus_Success) {
			printf("Error: PUF enrollment failed!\r\n");
			flash_area_close(flash_area);
			return -1;
		}
		printf("PUF Enroll successful. Activation Code created.\r\n");

		/* Create a padded buffer to meet flash alignment requirements */
		uint8_t padded_buf[aligned_size];
		/* Fill with erased flash value (typically 0xFF) */
		memset(padded_buf, 0xFF, aligned_size);
		/* Copy the activation code into the padded buffer */
		memcpy(padded_buf, activation_code, sizeof(activation_code));

		/* Erase the flash region; note that erase size must be aligned */
		err = flash_erase(flash_dev, absolute_offset, aligned_size);
		if (err != 0) {
			printf("Error: Flash erase failed! (rc %d)\r\n", err);
			flash_area_close(flash_area);
			return -1;
		}

		/* Write the activation code using the flash driver */
		err = flash_write(flash_dev, absolute_offset, padded_buf, aligned_size);
		if (err != 0) {
			printf("Error: Flash write failed! (rc %d)\r\n", err);
			flash_area_close(flash_area);
			return -1;
		}
		printf("Activation code saved in flash.\r\n");

		// Deinitialize and reinitialize the PUF after enrollment
		PUF_Deinit(PUF, &pufConfig);
		PUF_Init(PUF, &pufConfig);
		printf("PUF Reinitialized after enrollment.\r\n");
		}
	else {
		// Read the activation code from flash
		uint8_t padded_buf[aligned_size];
		memset(padded_buf, 0xFF, aligned_size);
		/* Read the full padded area from flash */
		err = flash_read(flash_dev, absolute_offset, padded_buf, aligned_size);
		if (err != 0) {
			printf("Error: Flash read failed! (rc %d)\r\n", err);
			flash_area_close(flash_area);
			return -1;
		}
		/* Extract the activation code from the padded buffer */
		memcpy(activation_code, padded_buf, sizeof(activation_code));
		printf("Activation code read from flash.\r\n");
	}
	// Start the PUF
	status = PUF_Start(PUF, activation_code, sizeof(activation_code));
	if (status != kStatus_Success) {
		printf("Error: PUF start failed!\r\n");
		return -1;
	}
	printf("PUF Started successfully.\r\n");
	flash_area_close(flash_area);
	return 0;
}
