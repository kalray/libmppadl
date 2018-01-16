#include <mppa/osconfig.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mppa_dl_lib.h>

extern const char empty_module_unload_free_mdo[];

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
    
  if (mppa_dl_load_buffer(handle, empty_module_unload_free_mdo) == -1) {
    fprintf(stderr, "mppa_dl_load_buffer failed: %s\n", mppa_dl_errstr(handle));
    status = -1;
    goto fail;
  }
  printf("Module loaded, ok\n");

  if (mppa_dl_unload(handle) == -1) {
    fprintf(stderr, "mppa_dl_unload failed: %s\n", mppa_dl_errstr(handle));
    status = -1;
    goto fail;
  }

  printf ("Module unloaded correctly\n");

  if (mppa_dl_handle_delete(handle) == -1) {
    fprintf(stderr, "mppa_dl_free failed: %s\n", mppa_dl_errstr(handle));
    status = -1;
    goto fail;
  }

  printf ("Module freed correctly\n");

 fail:
  return status;
}
