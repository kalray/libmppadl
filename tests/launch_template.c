#include <mppa/osconfig.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "mppa_dl_lib.h"
#include "test.h"

extern int sub_main(mppa_dl_handle_t **handles, int nb, const my_file_desc_t **descs) __attribute__((weak));

int magic_symbol_used_for_reloc = 0xDEADBEEF;

int self_contained_magic_func_increment_used_for_reloc(int arg){
  return arg + 1;
}

int main(int argc, char *argv[])
{
  mppa_dl_handle_t *handles[256];
  int handles_nb = 0;

  int status = 0;
  void *f_ptr;//, *d_ptr;

  const my_file_desc_t **f_desc_p = all_files_desc;

  while(*f_desc_p){
    mppa_dl_handle_t *handle;

    handle = mppa_dl_handle_new(mppa_dl_this(), 0);
    if (handle == NULL) {
      fprintf(stderr, "mppa_dl_handle_new failed\n");
      status = -1;
      goto mppa_dl_new_failed;
    }
    handles[handles_nb++] = handle;
    
    if (mppa_dl_load_file_generic(handle, (*f_desc_p)->name, &my_fopen_func, &my_fclose_func, &my_read_func) == -1) {
      fprintf(stderr, "mppa_dl_open failed: %s\n", mppa_dl_errstr(handle));
      status = -1;
      goto mppa_dl_open_failed;
    }

    /* if ((d_ptr = mppa_dl_sym(handle, "printf_ptr")) != NULL) { */
    /*   *((void **)d_ptr) = (void *)&printf; */
    /* } */
    f_desc_p++;
  }

  if (sub_main != NULL){
    printf("sub main found\n");
    sub_main(handles, handles_nb, all_files_desc);
  }

  int i;
  for(i=0; i<handles_nb; i++){
    if ((f_ptr = mppa_dl_sym(handles[i], "mppa_dl_main")) == NULL) {
      printf("No mppa_dl_main found in %s (%d)\n", all_files_desc[i]->name,i);
    } else {
      printf("Found mppa_dl_main in %s (%d) at %p\n", all_files_desc[i]->name,i, f_ptr);
      printf("Module load address is %p\n", mppa_dl_load_addr(handles[i]));

      status = ((int (*)(int, char **))f_ptr)(argc, argv);
    }
  }
  
  printf("Returned status: %d\n", status);
  // mppa_dl_sym_failed:
 /*  mppa_dl_unload(handle); */

 mppa_dl_open_failed:
 /*  mppa_dl_handle_delete(handle); */

 mppa_dl_new_failed:
  return status;
}
