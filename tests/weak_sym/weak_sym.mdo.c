// expected value 0
extern int *this_is_weak_and_should_be_0 __attribute__((weak));

// expected value != NULL and pointing to 0xdeadbeef
extern int *this_is_weak_but_bound_to_main __attribute__((weak));

// Following 2 symbols are set from linker parameter --defsym

// expected value == 0xcafebebe
extern char this_is_weak_bound_to_main_from_cli_nonull __attribute__((weak));

// expected value == NULL
extern char this_is_weak_bound_to_main_from_cli __attribute__((weak));

int
mppa_dl_main(int argc, char *argv[])
{
  return !((this_is_weak_and_should_be_0 == 0)
	   && &this_is_weak_bound_to_main_from_cli_nonull == (void*)0xcafebebe
	   && &this_is_weak_bound_to_main_from_cli == 0
	   && (this_is_weak_but_bound_to_main != 0 && *this_is_weak_but_bound_to_main == 0xdeadbeef));
}
