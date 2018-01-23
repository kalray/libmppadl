#include <mppa/osconfig.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <mppa_dl.h>

#include "generated/simple_printf_reloc_hex.h"

int main(int argc, char *argv[])
{
	int status = 0;
	void *f_ptr, *d_ptr;

	mppa_dl_handle_t *handle;

	handle = mppa_dl_load(simple_printf_reloc_mdo);
	if (handle == NULL) {
		fprintf(stderr, "mppa_dl_load failed\n");
		status = -1;
		goto fail;
	}

	if ((d_ptr = mppa_dl_sym(handle, "printf_ptr")) != NULL) {
		printf("Setting printf_ptr to &printf within loaded module\n");
		*((void **)d_ptr) = (void *)&printf;
	} else {
		fprintf(stderr, "Failed to find printf_ptr symbol in dyn object\n");
		status = -1;
		goto fail;
	}

	if ((f_ptr = mppa_dl_sym(handle, "mppa_dl_main")) == NULL) {
		fprintf(stderr, "No mppa_dl_main found\n");
		status = -1;
		goto fail;
	} else {
		printf("Found mppa_dl_main at {{%p}}\n", f_ptr);
		printf("Module load address is {{%p}}\n", mppa_dl_load_addr(handle));
		printf("Jumping ! \n");
		status = ((int (*)(int, char **))f_ptr)(argc, argv);
	}

	printf("Returned status: %d\n", status);

fail:
	return status;
}
