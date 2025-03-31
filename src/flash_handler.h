#include <zephyr/drivers/flash.h>
#include <zephyr/storage/flash_map.h>

/* Flash configuration macros, specific to
 * platform/use-case. These were based
 * on the LPC55S69 board */

#ifndef FLASH_WRITE_SIZE
#define FLASH_WRITE_SIZE  0x200 /* 512 bytes as per DTS */
#endif
#ifndef PARTITION_OFFSET
#define PARTITION_OFFSET  0     /* Offset within the partition */
#endif
#ifndef STORAGE_PARTITION
#define STORAGE_PARTITION storage_partition
#endif

/* Define flash layout offsets within the partition */
#define FLASH_OFFSET_ACTIVATION_CODE (PARTITION_OFFSET + 0)
#define FLASH_SIZE_ACTIVATION_CODE   ALIGN_UP(PUF_ACTIVATION_CODE_SIZE, FLASH_WRITE_SIZE)

/* Later additional variables can be tracked as needed, for example:
 * #define FLASH_OFFSET_ADDITIONAL_VAR (FLASH_OFFSET_ACTIVATION_CODE + FLASH_SIZE_ACTIVATION_CODE)
 * #define FLASH_SIZE_ADDITIONAL_VAR   0x400
 */

/* Macro to align size up to the next multiple of 'align' */
#define ALIGN_UP(x, align) (((x) + (align) - 1) & ~((align) - 1))
