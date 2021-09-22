// lexer

// source must be terminated by a '\0' character

static tk_vec_t * lx_lex(char const * source) {
  (void) source;

  tk_buf_t buf = tk_buf_make();

  return tk_buf_vec(&buf);
}
