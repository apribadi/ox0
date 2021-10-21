#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.c"
#include "memory.c"
#include "symbol.c"
#include "sexp.c"
#include "syntax.c"
#include "codegen.c"
#include "lexer.c"
#include "parser.c"
#include "io.c"

int main(int argc, const char * argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: ox [path]\n");
    return 1;
  }

  char const * filename = argv[1];
  char const * source = io_read(filename);

  Arena arena = arena_make();
  Parser parser = parser_make(&arena, filename, source);

  Sexp e = parser_expression(&parser);

  sexp_show(e);

  /*
  lx_t lex = lx_make(source);

  for (tk_t tok = lx_step(&lex); tok.tag != TK_EOF; tok = lx_step(&lex)) {
    tk_show(tok);
  }
  */

  return 0;
}
