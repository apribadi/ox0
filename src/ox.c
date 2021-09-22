#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "type.c"
#include "util.c"
#include "memory.c"
#include "bc.c"
#include "vm.c"

int main(int argc, const char * argv[]) {
  (void) argc;
  (void) argv;

  printf("Hello!\n");

  panic("panic!");

  return 0;
}
