#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mppa_dl.h>

#include "tls_hex.h"

int main(int argc, char *argv[])
{
	void *handle;
	int status = 0;

	handle = mppa_dl_load(tls_mdo, MPPA_DL_LOCAL);
	if (handle != NULL) {
		fprintf(stderr, "mppa_dl_load did not fail\n");
		status = -1;
		goto fail;
	}
	printf("mppa_dl did not load the module: this is correct: %s\n",
	       mppa_dl_error ());

fail:
	return status;
}
