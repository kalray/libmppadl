#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mppa_dl.h>

#include "jumptable_hex.h"

#define MAIN_NAME "main_dl_jumptable"

int main(int argc, char *argv[])
{
	void *handle;
	int status = 0;
	void *f_ptr;

	handle = mppa_dl_load(jumptable_mdo, MPPA_DL_LOCAL);
	if (handle == NULL) {
		fprintf(stderr, "mppa_dl_load failed: %s\n", mppa_dl_error());
		status = -1;
		goto fail;
	}

	if ((f_ptr = mppa_dl_sym(handle, MAIN_NAME)) == NULL) {
		fprintf(stderr, "No " MAIN_NAME " found in jumptable module\n");
		goto fail;
	} else {
		printf("Found " MAIN_NAME " in jumptable module at {{%p}}\n",  f_ptr);
		printf("Module load address is {{%p}}\n", mppa_dl_load_addr(handle));

		/* Make several calls to check we are executing correctly. */
		int (*fp)(int, int (*printfunc) (const char *name, ...))  = f_ptr;

		printf("First call\n");
		status = (fp(0, printf) == 1<<2);
		printf("Second call\n");
		status &= (fp(1, printf) == 2<<3);
		printf("Third call\n");
		status &= (fp(2, printf) == 3<<4);

	}

	printf("Returned status: %s\n", status ? "SUCCESS" : "FAILED");

fail:
	return !status;
}
