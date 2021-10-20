#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.c"
#include "memory.c"
#include "sexp.c"
#include "io.c"
#include "lex.c"
#include "parse.c"

int main(int argc, const char * argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: ox [path]\n");
    return 1;
  }

  char const * filename = argv[1];
  char const * source = io_read(filename);

  Arena arena = mm_arena_make();
  Parse parse = parse_make(&arena, filename, source);

  Sexp e = parse_expression(&parse);

  sexp_show(e);

  /*
  lx_t lex = lx_make(source);

  for (tk_t tok = lx_step(&lex); tok.tag != TK_EOF; tok = lx_step(&lex)) {
    tk_show(tok);
  }
  */

  return 0;
}
