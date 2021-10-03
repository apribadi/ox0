#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.c"
#include "mm.c"
#include "aa.c"
#include "sx.c"
#include "bc.c"
#include "vm.c"
#include "io.c"
#include "tk.c"
#include "lx.c"
#include "pa.c"
#include "test.c"

int main(int argc, const char * argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: ox [path]\n");
    return 1;
  }

  char * filename = (char *) argv[1];
  char * source = io_read(filename);

  aa_t arena = aa_make();
  pa_t parser = pa_make(&arena, filename, source);

  sx_t e = pa_expression(&parser);

  sx_show(e);

  /*
  lx_t lex = lx_make(source);

  for (tk_t tok = lx_step(&lex); tok.tag != TK_EOF; tok = lx_step(&lex)) {
    tk_show(tok);
  }
  */

  return 0;
}
