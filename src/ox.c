#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.c"
#include "mm.c"
#include "bc.c"
#include "vm.c"
#include "io.c"
#include "test.c"

int main(int argc, const char * argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: ox [path]\n");
    return 1;
  }

  char const * path = argv[1];
  char * text = io_read(path);

  (void) text;

  /*

  (void) argc;
  (void) argv;

  vm_t vm;

  vm_init(&vm);

  bc_prog_t * prog = test_1();

  bc_prog_show(prog);

  printf("\n");

  vm_result_t vm_result = vm_run(&vm, prog);

  vm_drop(&vm);

  (void) vm_result;
  */

  return 0;
}
