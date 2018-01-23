#include <mppa/osconfig.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mppa_dl.h>
#include <malloc.h>

#include "generated/empty_module_hex.h"

static int memalign_called = 0;
static int malloc_called = 0;
static int free_called = 0;

#ifdef __rtems__
static void* my_mppa_dl_memalign_func (size_t boundary, size_t size){
	void *b;
	memalign_called++;
	int r = posix_memalign(&b, boundary, size);
	if (r != 0) {
		return NULL;
	} else {
		return b;
	}
}
#else
static void* my_mppa_dl_memalign_func (size_t boundary, size_t size){
  memalign_called++;
  return memalign(boundary, size);
}
#endif /* __rtems__ */

static void* my_mppa_dl_malloc_func (size_t size){
  malloc_called++;
  return malloc(size);
}

static void my_mppa_dl_free_func (void *ptr){
  free_called++;
  free(ptr);
}

int main(int argc, char *argv[])
{
  mppa_dl_handle_t *handle;
  int status = 0;

  mppa_dl_set_memalign(my_mppa_dl_memalign_func);
  mppa_dl_set_malloc(my_mppa_dl_malloc_func);
  mppa_dl_set_free(my_mppa_dl_free_func);

  handle = mppa_dl_load(empty_module_mdo);
  if (handle == NULL) {
    fprintf(stderr, "mppa_dl_load failed: %s\n", mppa_dl_error());
    status = -1;
    goto fail;
  }
    
  printf("Module loaded, ok\n");

  if (mppa_dl_unload(handle) == -1) {
    fprintf(stderr, "mppa_dl_unload failed: %s\n", mppa_dl_error());
    status = -1;
    goto fail;
  }

  if (memalign_called == 0){
    printf("custom memalign not called\n");
    status = -1;
  }
  if (malloc_called == 0){
    printf("custom malloc not called\n");
    status = -1;
  }
  if (free_called == 0){
    printf("custom free not called\n");
    status = -1;
  }

  if (status == 0){
    printf("callback called as expected\n");
  }
 fail:
  return status;
}
