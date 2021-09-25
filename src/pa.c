// parsing

typedef struct {
  lx_t lex;
  tk_t tok;
  tk_t next;
} pa_t;

static pa_t pa_make(char * source) {
  pa_t t;

  t.lex = lx_make(source);
  t.tok = lx_next(&t.lex);

  return t;
}

static void pa_advance(pa_t * t) {
  t->tok = lx_next(&t->lex);
}

static void pa_consume(pa_t * t, tk_tag_t tag) {
  if (t->tok.tag != TK_TAG) panic("pa_consume: tag mismatch!");

  pa_advance(t);
}

static void pa_number(pa_t * t) {
  i64 n = strtol(t->prev.start, NULL, 10);

  (void) n;
}

static void pa_grouping(pa_t * t) {
  pa_expression(t);
  pa_consume(t, TK_RPAREN);
}
