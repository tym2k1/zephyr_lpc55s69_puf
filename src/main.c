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

	return 0;
}
