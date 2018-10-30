#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mppa_dl.h>

#include "sym_reloc_hex.h"

// this symbol should be used by loaded module
int magic_symbol_used_for_reloc = 0xDEADBEEF;

int main(int argc, char *argv[])
{
	mppa_dl_handle_t *handle;
	int status = 0;
	void *f_ptr;

	handle = mppa_dl_load(sym_reloc_mdo, MPPA_DL_LOCAL);
	if (handle == NULL) {
		fprintf(stderr, "mppa_dl_load failed\n");
		status = -1;
		goto fail;
	}

	if ((f_ptr = mppa_dl_sym(handle, "mppa_dl_main")) == NULL) {
		fprintf(stderr, "No mppa_dl_main found in sym_reloc module\n");
		goto fail;
	} else {
		printf("Found mppa_dl_main in sym_reloc module at {{%p}}\n",  f_ptr);
		printf("Module load address is {{%p}}\n", mppa_dl_load_addr(handle));

		status = ((int (*)(int, char **))f_ptr)(argc, argv);
	}

	printf("Returned status: %d\n", status);

fail:
	return status;
}
