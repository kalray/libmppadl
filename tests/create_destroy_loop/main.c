#include <mppa/osconfig.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mppa_dl.h>

#include "generated/create_destroy_hex.h"
#define NB_LOOPS 100

int main(int argc, char *argv[])
{
	void *handle;
	int status = 0;

    
	int i;
	for (i=1; i <= NB_LOOPS; i++) {
		if (!(i%NB_LOOPS)) {
			printf ("Iteration %d\n", i);
		}

		handle = mppa_dl_load(create_destroy_mdo, sizeof(create_destroy_mdo));
		if (handle == NULL) {
			fprintf(stderr, "mppa_dl_load failed: %s\n", mppa_dl_error());
			status = -1;
			goto fail;
		}

		if (!(i%NB_LOOPS)){
			printf("Module loaded, ok\n");
		}

		if (mppa_dl_unload(handle) == -1) {
			fprintf(stderr, "mppa_dl_unload failed: %s\n", mppa_dl_error());
			status = -1;
			goto fail;
		}

		if (!(i%NB_LOOPS)){
			printf ("Module unloaded correctly\n");
		}

	}

fail:
	return status;
}
