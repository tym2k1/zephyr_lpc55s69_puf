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
const char *input_hex = "04F247AA4777B5368F6BAEFC4ACC10994D0C45EEA9E161A84912A77186560B86B0A420DDD38D68AB61BD968CD287C4CBD7E088C2703919E065B6A19861451D9A05";  // Paste your commitment string here

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


    /* Initialize flash area and device */
    ret = flash_initialize(FIXED_PARTITION_ID(STORAGE_PARTITION), &flash_area, &flash_dev);
    if (ret != 0) {
        printf("Flash Initialization failed!\r\n");
        return ret;
    }
    
    // Initialization of PUF
    ret = initialize_and_start_puf(PUF,pufConfig,
                                    activation_code,
                                    PUF_ACTIVATION_CODE_SIZE,
                                    flash_area,
                                    flash_dev,
                                    ENROLLMENT_IS_UP);
    
    if(ret!=0){
        printf("PUF Initialisation Failed\r\n");
        return ret;
    }

    mbedtls_ecp_group grp;
	mbedtls_ecp_point h, C;
	
    ret = init_ECC(&grp,&h,&C);
    if(ret!=0)
    {
        printf("ECC couldn't be initiliased\r\n");
        return ret;
    }

    if(ENROLLMENT_IS_UP==1){

        ret = perform_enrollment(&grp,&h,&C,c1,CHALLENGE_SIZE, c2, CHALLENGE_SIZE, 
                            PUF,
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

        uint8_t commitment[COMMITMENT_BUFFER_SIZE];
        
        // Convert the hex string to binary buffer
        int ret = hex_string_to_bytes(input_hex, commitment, sizeof(commitment));
        if (ret != 0) {
            printf("Invalid hex string or buffer size too small\n");
            return -1;
        }

        printf("Commitment received (in hex):\n");
        for (size_t i = 0; i < sizeof(commitment); i++) {
            printf("%02x ", commitment[i]);
        }
        printf("\n");

        // Deserialize commitment into C
        ret = import_commitment(&grp,commitment, &C);
        if (ret != 0) {
            printf("Failed to import commitment\n");
            return ret;
        }

        mbedtls_ecp_point proof;
        mbedtls_ecp_point_init(&proof);
        mbedtls_mpi result_v, result_w, nonce;
        mbedtls_mpi_init(&result_v);
        mbedtls_mpi_init(&result_w);
        mbedtls_mpi_init(&nonce);

        ret = perform_authentication(&grp, &grp.G, &h, &proof, &C, &result_v, &result_w, &nonce , c1, CHALLENGE_SIZE, c2, CHALLENGE_SIZE, 
                            PUF,
                            activation_code,
                            PUF_ACTIVATION_CODE_SIZE,
                            flash_area,
                            flash_dev);

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

    puf_deinit(PUF,pufConfig); // Deinitalization Of Puf
    
    return 0;
}
