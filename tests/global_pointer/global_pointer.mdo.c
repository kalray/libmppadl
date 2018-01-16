
int foo(int i, int j)
{
  return i<<j;
}

int (*bar)(int i, int j) = &foo;

int
mppa_dl_main(int argc, char *argv[])
{
  int diff = 0;
  
  diff = bar(0xdeadbeef, 2);
    
  return !(diff == 0x7AB6FBBC);
}
