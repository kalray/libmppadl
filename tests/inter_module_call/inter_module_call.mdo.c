#include <stdio.h>
#include <mppa_dl_lib.h>

int
mppa_dl_main(int argc, char *argv[])
{
  int (*other_main_ptr)(int argc, char*argv[]);

  other_main_ptr = mppa_dl_sym_rec(mppa_dl_this(), "other_main");

  if (other_main_ptr == NULL) {
    // this is expected, as other_main_ptr is located in a sibling module.
    return 0;
  } else {
    return -1;
  }
}
