#include <mppa/osconfig.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mppa_dl_lib.h>

extern const char inter_module_call_mdo[], other_module_mdo[];

int main(int argc, char *argv[])
{
  mppa_dl_handle_t *handle1, *handle2;
  int status = 0;
  void *f_ptr;

  handle1 = mppa_dl_handle_new(mppa_dl_this(), 0);
  if (handle1 == NULL) {
    fprintf(stderr, "mppa_dl_handle_new failed\n");
    status = -1;
    goto fail;
  }
    
  if (mppa_dl_load_buffer(handle1, inter_module_call_mdo) == -1) {
    fprintf(stderr, "mppa_dl_load_buffer failed: %s\n", mppa_dl_errstr(handle1));
    status = -1;
    goto fail;
  }

  handle2 = mppa_dl_handle_new(mppa_dl_this(), 0);
  if (handle2 == NULL) {
    fprintf(stderr, "mppa_dl_handle_new failed\n");
    status = -1;
    goto fail;
  }
    
  if (mppa_dl_load_buffer(handle2, other_module_mdo) == -1) {
    fprintf(stderr, "mppa_dl_load_buffer failed: %s\n", mppa_dl_errstr(handle2));
    status = -1;
    goto fail;
  }

  if ((f_ptr = mppa_dl_sym(handle1, "mppa_dl_main")) == NULL) {
    fprintf(stderr, "No mppa_dl_main found in inter_module_call module\n");
    goto fail;
  } else {
    printf("Found mppa_dl_main in inter_module_call module at {{%p}}\n",  f_ptr);
    printf("Module load address is {{%p}}\n", mppa_dl_load_addr(handle1));

    status = ((int (*)(int, char **))f_ptr)(argc, argv);
  }

  printf("Returned status: %d\n", status);

 fail:
  return status;
}
