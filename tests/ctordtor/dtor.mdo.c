
static int global = 9;

__attribute__((destructor(800)))
static void
this_is_some_ctor (void) 
{
  global = 10;
}


int
mppa_dl_main(int argc, char *argv[])
{
  return global == 10;
}
