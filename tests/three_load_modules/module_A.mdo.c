#include <mppa_dl.h>

#include "module_B_hex.h"

extern int printf(const char *fmt, ...);

int
moduleA_main(int val) {
	mppa_dl_handle_t *sub_module;

	sub_module = mppa_dl_load(module_B_mdo);
	if ( sub_module == NULL){
		printf("[module A] Error loading submodule module_B from module_A\n");
		return 0;
	}


	void *f_ptr;
	if ((f_ptr = mppa_dl_sym(sub_module, "module_B_main")) == (void*)0) {
		printf("[module A] Error when looking for entry point in module_B\n");
		return 0;
	}

	printf("[module A] Found module_B_main in %s at {{%p}}\n", "moduleB", f_ptr);
	printf("[module A] Module load address is {{%p}}\n", mppa_dl_load_addr(sub_module));

	printf ("[module A] Jumping in module B (this printf has been dynamically resolved)\n");
	int ret = ((int (*)(void))f_ptr)();

	return ret ^ val;
}
