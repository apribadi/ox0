#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "type.c"
#include "util.c"
#include "memory.c"
#include "bytecode.c"

int main(int argc, const char * argv[]) {
  (void) argc;
  (void) argv;

  printf("Hello!\n");

  panic("panic!");

  return 0;
}
