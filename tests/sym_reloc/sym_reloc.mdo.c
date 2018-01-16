#include <stdio.h>
#include <stdlib.h>

extern int mppa_dl_main(int argc, char *argv[]);
extern int magic_symbol_used_for_reloc;

int
mppa_dl_main(int argc, char *argv[])
{
  return !(magic_symbol_used_for_reloc == 0xDEADBEEF);
}
