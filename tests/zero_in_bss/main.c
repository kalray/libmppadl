#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mppa_dl.h>

#include "zero_in_bss_hex.h"

int main(int argc, char *argv[])
{
	mppa_dl_handle_t *handle;
	int status = 0;
	void *f_ptr;

	handle = mppa_dl_load(zero_in_bss_mdo, MPPA_DL_LOCAL);
	if (handle == NULL) {
		fprintf(stderr, "mppa_dl_load failed\n");
		status = -1;
		goto fail;
	}

	if ((f_ptr = mppa_dl_sym(handle, "mppa_dl_main")) == NULL) {
		fprintf(stderr, "No mppa_dl_main found in zero_in_bss module\n");
		goto fail;
	} else {
		printf("Found mppa_dl_main in zero_in_bss module at {{%p}}\n",  f_ptr);
		printf("Module load address is {{%p}}\n", mppa_dl_load_addr(handle));

		status = ((int (*)(int, char **))f_ptr)(argc, argv);
	}

	printf("Returned status: %d\n", status);
	if (status != 0) {
		printf("Failed: zero_in_bss should return zero, but not %d\n", status);
	} else {
		// Correct
		status = 0;
	}

fail:
	return status;
}
