extern int mppa_dl_main(int argc, char *argv[]);

/*
 * Function pointers needed.
 */
int (*printf_ptr)(const char *fmt, ...);

int
mppa_dl_main(int argc, char *argv[])
{
  printf_ptr("Hello World from a submodule!\n");
  printf_ptr("End, returning from mppa_dl_main\n");

  return 0;
}
