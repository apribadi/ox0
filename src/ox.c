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
#include "test.c"

int main(int argc, const char * argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: ox [path]\n");
    return 1;
  }

  u8 * source = io_read(argv[1]);

  lx_t lexer = lx_make(source);

  tk_t token = lx_lex(&lexer);

  while (token.tag != TK_EOF) {
    tk_show(token);
    token = lx_lex(&lexer);
  }

  return 0;
}
