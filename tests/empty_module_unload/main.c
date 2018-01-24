#include <mppa/osconfig.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mppa_dl.h>

#include "empty_module_unload_hex.h"

int main(int argc, char *argv[])
{
	void *handle;
	int status = 0;

	handle = mppa_dl_load(empty_module_unload_mdo);
	if (handle == NULL) {
		fprintf(stderr, "mppa_dl_load failed: %s\n", mppa_dl_error());
		status = -1;
		goto fail;
	}

	printf("Module loaded, ok\n");

	if (mppa_dl_unload(handle) == -1) {
		fprintf(stderr, "mppa_dl_unload failed: %s\n", mppa_dl_error());
		status = -1;
		goto fail;
	}

	printf ("Module unloaded correctly\n");

fail:
	return status;
}
