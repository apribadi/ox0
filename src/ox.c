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
#include "lexing.c"
#include "parsing.c"
#include "codegen.c"
#include "io.c"

int main(int argc, const char * argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: ox [path]\n");
    return 1;
  }

  char const * filename = argv[1];
  char const * source = io_read(filename);

  Arena arena = arena_make();
  Parser parser = make_parser(&arena, filename, source);

  Expr e;
 
  if (parse_expr(&parser, &e)) return 0;

  codegen_emit_function(e);

  return 0;
}
