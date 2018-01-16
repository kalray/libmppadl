extern int printf(const char *fmt, ...);
extern int self_contained_magic_func_increment_used_for_reloc(int arg);

int
mppa_dl_main(int argc, char *argv[])
{
  printf("This printf execution uses a PLT within a dynamic module\n");
  int should_be_48 = self_contained_magic_func_increment_used_for_reloc(47);
  
  return !(should_be_48 == 48);
}
