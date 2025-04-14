#include <stdio.h>
#include "flash_handler.h"
#include "puf_prover.h"
#include "mbedtls/ecp.h"

#define ENROLLMENT_IS_UP 0

int challengeSize = 16;

const uint8_t c1[16] = {
    0x3C, 0xA1, 0xF4, 0x92,
    0x57, 0xB8, 0x0E, 0x6D,
    0x1F, 0xA9, 0xC3, 0xE7,
    0x74, 0x90, 0x12, 0xAD
};

const uint8_t c2[16] = {
    0xFF, 0x00, 0xFF, 0x00,
    0xFF, 0x00, 0xFF, 0x00,
    0x00, 0xFF, 0x00, 0xFF,
    0x00, 0xFF, 0x00, 0xFF
};


int main(void)
{
    __attribute__((aligned(16))) uint8_t activation_code[PUF_ACTIVATION_CODE_SIZE];

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
    ret = initializeAndStartPuf(PUF,pufConfig,
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
	
    ret = performEnrollment(&grp,&h,&C,c1,challengeSize, c2, challengeSize, 
                          PUF,
                          activation_code,
                          PUF_ACTIVATION_CODE_SIZE,
                          flash_area,
                          flash_dev,
                          ENROLLMENT_IS_UP);

    if(ret!=0){
        printf("Enrollment Failed");
        return ret;
    }
    else
    {
        printf("Enrollment Success");
    }

    pufDeinit(PUF,pufConfig); // Deinitalization Of Puf
    
    return 0;
}
