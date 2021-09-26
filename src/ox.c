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
#include "tk.c"
#include "lx.c"
// #include "pa.c"
#include "test.c"

int main(int argc, const char * argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: ox [path]\n");
    return 1;
  }

  char * source = io_read(argv[1]);

  (void) source;

  /*

  pa_t parser = pa_make(source);

  pa_expression(&parser);
  */

  lx_t lex = lx_make(source);

  for (tk_t tok = lx_step(&lex); tok.tag != TK_EOF; tok = lx_step(&lex)) {
    tk_show(tok);
  }

  return 0;
}
