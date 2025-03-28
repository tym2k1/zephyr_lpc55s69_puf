#include <stdio.h>
#include "fsl_puf.h"

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

	// Enroll the PUF
	memset(activation_code, 0, sizeof(activation_code));
	status = PUF_Enroll(PUF, activation_code, sizeof(activation_code));
	if (status != kStatus_Success) {
		printf("Error: PUF enrollment failed!\r\n");
		return -1;
	}
	printf("PUF Enroll successful. Activation Code created.\r\n");

	return 0;
}
