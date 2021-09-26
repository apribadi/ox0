// parsing

typedef struct {
  lx_t lexer;
  tk_t token;
  tk_t lookahead;
} pa_t;

static pa_t pa_make(char * source) {
  pa_t t;

  t.lexer = lx_make(source);
  t.token = lx_step(&t.lexer);
  t.lookahead = lx_step(&t.lexer);

  return t;
}

static void pa_advance(pa_t * t) {
  t->token = t->lookahead;
  t->lookahead = lx_step(&t->lexer);
}

static void pa_consume(pa_t * t, tk_tag_t tag) {
  // TODO: real error handling
  if (t->token.tag != tag)
    panic("pa_consume: tag mismatch!");

  pa_advance(t);
}

static void pa_expression(pa_t * t) {
  printf("pa_expression\n");

  pa_consume(t, TK_NUMBER);
}

/*


static void pa_number(pa_t * t) {
  i64 n = strtol(t->prev.start, NULL, 10);

  (void) n;
}

static void pa_grouping(pa_t * t) {
  pa_expression(t);
  pa_consume(t, TK_RPAREN);
}
*/
