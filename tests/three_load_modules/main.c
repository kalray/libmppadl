#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "mppa_dl.h"

#include "module_A_hex.h"

int main(int argc, char *argv[])
{
	mppa_dl_handle_t *handle_moduleA;

	int status = -1;
	void *f_ptr;

	int val = 123456;

	handle_moduleA = mppa_dl_load(module_A_mdo, MPPA_DL_LOCAL);
	if (handle_moduleA == NULL) {
		fprintf(stderr, "mppa_dl_load failed\n");
		status = -1;
		goto mppa_dl_load_buffer_failed;
	}

	if ((f_ptr = mppa_dl_sym(handle_moduleA, "moduleA_main")) == NULL) {
		fprintf(stderr, "No mppa_dl_main found in module_A\n");
		goto mppa_dl_new_failed;
	} else {
		printf("Found mppa_dl_main in %s at {{%p}}\n", "moduleA", f_ptr);
		printf("Module load address is {{%p}}\n", mppa_dl_load_addr(handle_moduleA));
    
		status = ((int (*)(int))f_ptr)(val);
		printf ("Ret value : %x\n", status);
	}

mppa_dl_load_buffer_failed:
	mppa_dl_unload(handle_moduleA);

mppa_dl_new_failed:
	if (status == (0xdeadbeef ^ val)){
		return 0;
	} else {
		return -1;
	}
}
