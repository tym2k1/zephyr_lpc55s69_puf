#ifndef PUF_PROVER_H
#define PUF_PROVER_H

#include "fsl_puf.h"
#include "flash_handler.h"
#include "mbedtls/ecp.h"


#define NUM_RANDOM_NUMBERS 2
#define RANDOM_NUMBER_SIZE_BYTES 32
#define PUF_SLOT_0 0
#define PUF_SLOT_1 1
#define CONSTANT_FOR_H_GENERATOR 123456789

/**
 * @brief Initialize the PUF prover.
 *
 * This function initializes the PUF Prover logic and then
 * either enrolls and saves the activation code to flash (if writeToFlash is true)
 * or reads the activation code from flash.
 *
 * @param puf                  Pointer to the PUF instance.
 * @param activation_code      Buffer where the activation code will be stored.
 * @param activation_code_size Size of the activation code in bytes.
 * @param flash_dev            Pointer to the flash device to be used.
 * @param flash_area           Pointer to the flash area structure to be used.
 * @param writeToFlash         True to enroll and write the activation code,
 *                             false to read it from flash.
 *
 * @return 0 on success, negative error code on failure.
 */
int PUF_Prover_Initialize(PUF_Type *puf,
                          uint8_t *activation_code,
                          size_t activation_code_size,
                          const struct flash_area *flash_area,
                          const struct device *flash_dev,
                          bool writeToFlash);


int getPufKey(PUF_Type *puf,
                          uint8_t *activation_code,
                          size_t activation_code_size,
                          const struct flash_area *flash_area,
                          const struct device *flash_dev,
                          bool writeToFlash,
                          uint8_t key_slot,
                          uint8_t *out_key_buffer);

int initializeAndStartPuf(PUF_Type *puf,puf_config_t pufConfig,
                          uint8_t *activation_code,
                          size_t activation_code_size,
                          const struct flash_area *flash_area,
                          const struct device *flash_dev,
                          bool writeToFlash);

void pufDeinit(PUF_Type *puf,puf_config_t pufConfig);

int performEnrollment(mbedtls_ecp_group *grp, mbedtls_ecp_point *h, mbedtls_ecp_point *C , const uint8_t *c1, size_t c1_size, const uint8_t *c2, size_t c2_size , 
                          PUF_Type *puf,
                          uint8_t *activation_code,
                          size_t activation_code_size,
                          const struct flash_area *flash_area,
                          const struct device *flash_dev,
                          bool writeToFlash);

int getResponseToChallenge(const uint8_t *c, size_t c_size, mbedtls_mpi *r, PUF_Type *puf,
                          uint8_t *activation_code,
                          size_t activation_code_size,
                          const struct flash_area *flash_area,
                          const struct device *flash_dev,
                          bool writeToFlash,int pufSlot
                          );



#endif /* PUF_PROVER_H */
