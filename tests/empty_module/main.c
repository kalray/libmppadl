#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mppa_dl.h>

#include "empty_module_hex.h"

int main(int argc, char *argv[])
{
	void *handle;
	int status = 0;

	handle = mppa_dl_load(empty_module_mdo, MPPA_DL_LOCAL);
	if (handle == NULL) {
		fprintf(stderr, "mppa_dl_load failed: %s\n", mppa_dl_error());
		status = -1;
		goto fail;
	}

	printf("Module loaded, ok\n");

fail:
	return status;
}
