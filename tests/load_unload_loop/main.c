#include <mppa/osconfig.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mppa_dl_lib.h>

extern const char load_unload_mdo[];

#define NB_LOOPS 100

int main(int argc, char *argv[])
{
  mppa_dl_handle_t *handle;
  int status = 0;

  handle = mppa_dl_handle_new(mppa_dl_this(), 0);
  if (handle == NULL) {
    fprintf(stderr, "mppa_dl_handle_new failed\n");
    status = -1;
    goto fail;
  }
    
  int i;
  for (i=1; i <= NB_LOOPS; i++){
    if (!(i%100)){
      printf ("Iteration %d\n", i);
    }

    if (mppa_dl_load_buffer(handle, load_unload_mdo) == -1) {
      fprintf(stderr, "mppa_dl_load_buffer failed: %s\n", mppa_dl_errstr(handle));
      status = -1;
      goto fail;
    }

    if (!(i%100)){
      printf("Module loaded, ok\n");
    }

    if (mppa_dl_unload(handle) == -1) {
      fprintf(stderr, "mppa_dl_unload failed: %s\n", mppa_dl_errstr(handle));
      status = -1;
      goto fail;
    }

    if (!(i%100)){
      printf ("Module unloaded correctly\n");
    }
  }
  if (mppa_dl_handle_delete(handle) == -1) {
    fprintf(stderr, "mppa_dl_free failed: %s\n", mppa_dl_errstr(handle));
    status = -1;
    goto fail;
  }

  printf ("Module freed correctly\n");

 fail:
  return status;
}
