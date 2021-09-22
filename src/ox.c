#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "flag.c"
#include "type.c"
#include "panic.c"
#include "memory.c"
#include "bc.c"
#include "vm.c"
#include "test.c"

int main(int argc, const char * argv[]) {
  (void) argc;
  (void) argv;

  vm_t vm;

  vm_init(&vm);

  bc_prog_t * prog = test_1();

  vm_result_t vm_result = vm_run(&vm, prog);

  vm_drop(&vm);

  (void) vm_result;

  return 0;
}
