#include <mppa/osconfig.h>
#include <stdio.h>
#include "mppa_dl.h"

#include "module_A_hex.h"
#include "module_B_hex.h"

int main(int argc, char **argv)
{
	int status;

	void *handle_moduleA, *handle_moduleB;
	void *f_ptra, *f_ptrb;

	handle_moduleA = mppa_dl_load(module_A_mdo, MPPA_DL_GLOBAL);
	if (handle_moduleA == NULL) {
		fprintf(stderr, "mppa_dl_load failed\n");
		return -1;
	}

	handle_moduleB = mppa_dl_load(module_B_mdo, MPPA_DL_GLOBAL);
	if (handle_moduleB == NULL) {
		fprintf(stderr, "mppa_dl_load failed\n");
		return -1;
	}

	if ((f_ptra = mppa_dl_sym(handle_moduleA, "module_main")) == NULL) {
		fprintf(stderr, "No module_main found in module_A\n");
		return -1;
	} else {
		printf("Found module_main in %s at {{%p}}\n",
		       "moduleA", f_ptra);
		printf("Module load address is {{%p}}\n",
		       mppa_dl_load_addr(handle_moduleA));

		status = ((int (*)(int, char **))f_ptra)(argc, argv);
		printf ("Ret value (moduleA) : %x\n", status);
	}

	if ((f_ptrb = mppa_dl_sym(handle_moduleB, "module_main")) == NULL) {
		fprintf(stderr, "No module_main found in module_B\n");
		return -1;
	} else {
		printf("Found module_main in %s at {{%p}}\n",
		       "moduleB", f_ptrb);
		printf("Module load address is {{%p}}\n",
		       mppa_dl_load_addr(handle_moduleB));

		status = ((int (*)(int, char **))f_ptrb)(argc, argv);
		printf ("Ret value (moduleB) : %x\n", status);
	}

	mppa_dl_unload(handle_moduleA);
	mppa_dl_unload(handle_moduleB);
	return 0;
}
