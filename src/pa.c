// parsing

typedef struct {
  lx_t lex;
  tk_t tok;
  tk_t lookahead;
} pa_t;

static pa_t pa_make(char * source) {
  pa_t t;

  t.lex = lx_make(source);
  // t.tok = ??;
  t.lookahead = lx_step(&t.lex);

  return t;
}

static void pa_advance(pa_t * t) {
  printf("pa_advance\n");

  t->tok = t->lookahead;
  t->lookahead = lx_step(&t->lex);

  tk_show(t->tok);
}

static void pa_consume(pa_t * t, tk_tag_t tag) {
  // TODO: real error handling

  if (t->lookahead.tag != tag)
    panic("pa_consume: tag mismatch!");

  pa_advance(t);
}

enum {
  PA_PR_NONE,
  PA_PR_ASSIGNMENT,
  PA_PR_OR,
  PA_PR_AND,
  PA_PR_EQUALITY,
  PA_PR_COMPARISON,
  PA_PR_TERM,
  PA_PR_FACTOR,
  PA_PR_UNARY,
  PA_PR_PRIMARY,
};

typedef i64 pa_precedence_t;

static void pa_parse_precedence(pa_t * t, i64 n);

static pa_precedence_t pa_precedence(tk_tag_t);

static void pa_expression(pa_t * t) {
  printf("pa_expression\n");

  pa_parse_precedence(t, PA_PR_ASSIGNMENT);
}

static void pa_grouping(pa_t * t) {
  printf("pa_grouping\n");

  pa_expression(t);
  pa_consume(t, TK_RPAREN);
}

static void pa_unary(pa_t * t) {
  printf("pa_grouping\n");

  tk_t op = t->tok;

  pa_parse_precedence(t, PA_PR_UNARY);

  switch (op.tag) {
    case TK_NEG: printf("NEG\n"); break;
  }
}

static void pa_binary(pa_t * t) {
  printf("pa_binary\n");

  tk_t op = t->tok;

  pa_precedence_t precedence = pa_precedence(op.tag);

  pa_parse_precedence(t, precedence + 1);

  switch (op.tag) {
    case TK_ADD: printf("ADD\n"); break;
    case TK_SUB: printf("SUB\n"); break;
    case TK_MUL: printf("MUL\n"); break;
    case TK_DIV: printf("DIV\n"); break;
  }
}

static void pa_number(pa_t * t) {
  printf("pa_number:\n");

  tk_show(t->tok);
}

typedef void (* pa_rule_t) (pa_t *);

static pa_rule_t pa_prefix_rule(tk_tag_t tag) {
  switch (tag) {
    case TK_LPAREN:
      return pa_grouping;
    case TK_NEG:
      return pa_unary;
    case TK_NUMBER:
      return pa_number;
  }

  return NULL;
}

static pa_rule_t pa_infix_rule(tk_tag_t tag) {
  switch (tag) {
    case TK_ADD:
      return pa_binary;
    case TK_SUB:
      return pa_binary;
    case TK_MUL:
      return pa_binary;
    case TK_DIV:
      return pa_binary;
  }

  return NULL;
}

static pa_precedence_t pa_precedence(tk_tag_t tag) {
  switch (tag) {
    case TK_ADD:
      return PA_PR_TERM;
    case TK_SUB:
      return PA_PR_TERM;
    case TK_MUL:
      return PA_PR_FACTOR;
    case TK_DIV:
      return PA_PR_FACTOR;
  }

  return PA_PR_NONE;
}

static void pa_parse_precedence(pa_t * t, pa_precedence_t precedence) {
  printf("pa_parse_precedence\n");

  pa_advance(t);

  pa_rule_t prefix_rule = pa_prefix_rule(t->tok.tag);

  if (!prefix_rule) {
    panic("expected expression!");
    return;
  }

  prefix_rule(t);

  while (precedence <= pa_precedence(t->lookahead.tag)) {
    pa_advance(t);
    pa_rule_t infix_rule = pa_infix_rule(t->tok.tag);
    infix_rule(t);
  }
}

/*


static void pa_grouping(pa_t * t) {
  pa_expression(t);
  pa_consume(t, TK_RPAREN);
}
*/
