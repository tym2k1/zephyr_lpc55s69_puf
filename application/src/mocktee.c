#include <zephyr/drivers/tee.h>

#define TEE_IMPL_ID 1

int tee_add_shm(const struct device *dev, void *addr, size_t align, size_t size,
		uint32_t flags, struct tee_shm **shmp)
{
    return 0;
};


int tee_rm_shm(const struct device *dev, struct tee_shm *shm)
{
    return 0;
};

static int mocktee_get_version(const struct device *dev, struct tee_version_info *info)
{
	if (!info) {
		return -EINVAL;
	}

	info->impl_id = TEE_IMPL_ID;
	info->impl_caps = NULL;

        /* Definitions in zephyr/drivers/tee.h
         *
         * TEE_GEN_CAP_GP - GlobalPlatform compliant TEE
         * Not supported yet
         *
         * TEE_GEN_CAP_PRIVILEGED - Privileged device (for supplicant)
         * Not relevant as we dont use privileged/unprivileged context
         *
         * TEE_GEN_CAP_REG_MEM - Supports registering shared memory
         * Non-supported, Shared memory is set statically at compile time by hypervisor
         *
         * TEE_GEN_CAP_MEMREF_NULL - Support NULL MemRef
         * TBD
         * This flag declares that TEE supports “NULL MemRef” parameters—that is,
         * when a Trusted Application (TA) is invoked it may legitimately see a
         * memref parameter whose buffer pointer is NULL.
         * Not all TEE implementations honor this: some will reject any NULL buffer with an error.
         *
         * */
	info->gen_caps = NULL;

	return 0;
}

static const struct tee_driver_api mocktee_api = {
    .get_version   = mocktee_get_version,
    .open_session  = tee_open_session,
    .close_session = tee_close_session,
    .cancel        = tee_cancel,
    .invoke_func   = tee_invoke_func,
    .shm_register  = tee_shm_register,
    .shm_unregister= tee_shm_unregister,
    .suppl_recv    = tee_suppl_recv,
    .suppl_send    = tee_suppl_send,
};

DEVICE_DEFINE(mocktee,                    /* C symbol for your device */
              "mocktee",                  /* the string name used by device_get_binding() */
              NULL,                       /* init function */
              NULL,                       /* pm_control (optional) */
              NULL,                       /* driver data (struct, if you need) */
              NULL,                       /* driver config (if any) */
              POST_KERNEL,                /* init level: when in boot-up */
              CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
              &mocktee_api);              /* pointer to your tee_driver_api */
