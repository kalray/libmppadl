#include <mppa/osconfig.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mppa_dl.h>

#include "generated/char_s_hex.c"
//extern const char char_s_mdo[];

int main(int argc, char *argv[])
{
	void *handle;
	int status = 0;
	void *f_ptr;

	handle = mppa_dl_load(char_s_mdo, sizeof(char_s_mdo));
	if (handle == NULL) {
		fprintf(stderr, "mppa_dl_load failed: %s\n", mppa_dl_error());
		status = -1;
		goto fail;
	}

	if ((f_ptr = mppa_dl_sym(handle, "mppa_dl_main")) == NULL) {
		fprintf(stderr, "No mppa_dl_main found in char_s module\n");
		goto fail;
	} else {
		printf("Found mppa_dl_main in char_s module at {{%p}}\n",  f_ptr);
		printf("Module load address is {{%p}}\n", mppa_dl_load_addr(handle));

		status = ((int (*)(int, char **))f_ptr)(argc, argv);
	}

	printf("Returned status: %d\n", status);

fail:
	return status;
}
