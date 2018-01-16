#include <stdio.h>
#include "mppa_dl_lib.h"

int (*other_main)(int argc, char *argv[]);

int
mppa_dl_main(int argc, char *argv[])
{
  int r = other_main(0, NULL);
  return r == 1234 ? 0 : 1;
}
