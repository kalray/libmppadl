#include <mppa_dl.h>

extern int printf(const char *fmt, ...);

int
module_main(int argc, char **argv)
{
	printf("[module A] Hello!\n");
	return 0;
}
