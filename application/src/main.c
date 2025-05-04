#include <stdio.h>
#include "flash_handler.h"
#include "puf_prover.h"
#include "mbedtls/ecp.h"
#include "mbedtls/memory_buffer_alloc.h"
#include "utils.h"

#define MEMORY_BUF_SIZE 16384
static unsigned char memory_buf[MEMORY_BUF_SIZE];

#define ENROLLMENT_IS_UP 1

#define COMMITMENT_BUFFER_SIZE 65
const char *commitment_hex = "0485E41D73D71C62636D6D8FCE2349382FB2FBCFD618229431957B551F8F3F84B9EFC43EAF541F981F05AB97820F0DE552E77F5924311A3450F37BD0DC9AD16693";  // Paste your commitment string here

#define CHALLENGE_SIZE 16


const uint8_t c1[CHALLENGE_SIZE] = {
    0x3C, 0xA1, 0xF4, 0x92,
    0x57, 0xB8, 0x0E, 0x6D,
    0x1F, 0xA9, 0xC3, 0xE7,
    0x74, 0x90, 0x12, 0xAD
};

const uint8_t c2[CHALLENGE_SIZE] = {
    0xFF, 0x00, 0xFF, 0x00,
    0xFF, 0x00, 0xFF, 0x00,
    0x00, 0xFF, 0x00, 0xFF,
    0x00, 0xFF, 0x00, 0xFF
};



int main(void)
{
    __attribute__((aligned(16))) uint8_t activation_code[PUF_ACTIVATION_CODE_SIZE];
    mbedtls_memory_buffer_alloc_init(memory_buf, sizeof(memory_buf));

    const struct flash_area *flash_area;
    const struct device *flash_dev;
    int ret;
    puf_config_t pufConfig;

    mbedtls_ecp_group grp;
	mbedtls_ecp_point h, C;

    // Initialize flash area and device 
    ret = flash_initialize(FIXED_PARTITION_ID(STORAGE_PARTITION), &flash_area, &flash_dev);
    if (ret != 0) {
        printf("Flash Initialization failed!\r\n");
        return ret;
    }
    
    
    if(ENROLLMENT_IS_UP==1){

        ret = perform_enrollment(&grp,&h,&C,c1,CHALLENGE_SIZE, c2, CHALLENGE_SIZE, 
                            PUF,pufConfig,
                            activation_code,
                            PUF_ACTIVATION_CODE_SIZE,
                            flash_area,
                            flash_dev,
                            ENROLLMENT_IS_UP);

        if(ret!=0){
            printf("Couldn't finish the enrollment\r\n");
            return ret;
        }
        else
        {
            printf("Enrollment was a success. Below you can find the commitment\r\n");
            
            uint8_t commitment_buffer[COMMITMENT_BUFFER_SIZE];

            size_t olen;
            olen = export_commitment(&grp,&C, commitment_buffer, sizeof(commitment_buffer));
            if (olen < 0) {
                printf("Error exporting commitment, error code: %d\n", olen);
                return 1;
            }

            
            printf("Commitment: ");
            for (size_t i = 0; i < COMMITMENT_BUFFER_SIZE; i++) {
                printf("%02X", commitment_buffer[i]);
            }
            printf("\n");
        }
    }

    else{

        mbedtls_ecp_point proof;
        mbedtls_ecp_point_init(&proof);
        mbedtls_mpi result_v, result_w, nonce;
        mbedtls_mpi_init(&result_v);
        mbedtls_mpi_init(&result_w);
        mbedtls_mpi_init(&nonce);

        ret = perform_authentication(&grp, &grp.G, &h, &proof, &C, &result_v, &result_w, &nonce , c1, CHALLENGE_SIZE, c2, CHALLENGE_SIZE, 
                            PUF,pufConfig,
                            activation_code,
                            PUF_ACTIVATION_CODE_SIZE,
                            flash_area,
                            flash_dev,commitment_hex, COMMITMENT_BUFFER_SIZE);

        if(ret!=0){
            printf("Couldn't Authenticate the device\r\n");
            return ret;
        }
        else
        {
            printf("Authentication was a success. Below you can find the variables to return\r\n");
            print_mpi("v", &result_v);
            print_mpi("w", &result_w);
            print_mpi("nonce", &nonce);
            print_ecp_point("proof", &grp, &proof);
        }

    }
    
    return 0;
}
