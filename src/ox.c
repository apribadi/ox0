#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.c"
#include "memory.c"
#include "sexp.c"
#include "syntax.c"
#include "lexer.c"
#include "parser.c"
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
  Parser parser = parser_make(&arena, filename, source);

  SyntaxExpression e = parser_expression(&parser);

  if (!parser.is_panicking) {
    codegen_emit_function(e);
  }

  return 0;
}
