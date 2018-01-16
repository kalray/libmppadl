extern int printf(const char *fmt, ...);

int
module_B_main(void)
{
  int val = 0xDEADBEEF;
  printf ("[module B] I'm module B, I'm returning %x (this printf has been dynamically resolved)\n", val);
  return val;
}
