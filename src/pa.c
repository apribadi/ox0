// parsing

typedef struct {
  lx_t lex;
  tk_t tok;
  tk_t lookahead;
  bool is_error;
  char * filename;
  char * source;
} pa_t;

enum {
  PA_PRECEDENCE_NONE,
  PA_PRECEDENCE_OR,
  PA_PRECEDENCE_AND,
  PA_PRECEDENCE_EQUALITY,
  PA_PRECEDENCE_COMPARISON,
  PA_PRECEDENCE_TERM,
  PA_PRECEDENCE_FACTOR,
  PA_PRECEDENCE_UNARY,
  PA_PRECEDENCE_PRIMARY,
};

typedef i64 pa_precedence_t;

// null denotation rule

typedef void (* pa_nud_rule_t) (pa_t *);

// left denotation rule

typedef void (* pa_led_rule_t) (pa_t *);

static pa_t pa_make(char * filename, char * source) {
  pa_t t;

  t.lex = lx_make(source);
  // t.tok = ??;
  t.lookahead = lx_step(&t.lex);
  t.is_error = false;
  t.filename = filename;
  t.source = source;

  return t;
}

static void pa_report_error(pa_t * t, char * location, char * message) {
  i64 line_number = 1;
  char * line_start = t->source;
  char * p = t->source;

  while (p != location) {
    if (* p == '\n') {
      line_number = line_number + 1;
      line_start = p + 1;
    }

    p ++;
  }

  char * q = line_start;

  while (* q != '\n' && * q != '\0') {
    q ++;
  }
  
  i64 line_length = q - line_start;
  i64 col = location - line_start;
  
  fprintf(
      stderr,
      "ERROR %s:%d:%d\n%.*s\n%*s^\n%*s%s\n",
      t->filename,
      (int) line_number,
      (int) col,
      (int) line_length,
      line_start,
      (int) col, "",
      (int) col, "",
      message
    );
};

static void pa_maybe_report_error(pa_t * t, char * location, char * message) {
  if (t->is_error) return;
  t->is_error = true;
  pa_report_error(t, location, message);
}

static void pa_advance(pa_t * t) {
  tk_t tok = t->lookahead;
  tk_t lookahead = lx_step(&t->lex);

  while (lookahead.tag == TK_ERROR) {
    pa_maybe_report_error(t, lookahead.start, "invalid token");
    lookahead = lx_step(&t->lex);
  }

  t->tok = tok;
  t->lookahead = lookahead;
}

static void pa_consume(pa_t * t, tk_tag_t tag) {
  if (t->lookahead.tag != tag) {
    pa_maybe_report_error(t, t->lookahead.start, "expected different token");
  };

  pa_advance(t);
}

static pa_precedence_t pa_precedence(tk_tag_t tag) {
  switch (tag) {
    case TK_ADD:
      return PA_PRECEDENCE_TERM;
    case TK_SUB:
      return PA_PRECEDENCE_TERM;
    case TK_MUL:
      return PA_PRECEDENCE_FACTOR;
    case TK_DIV:
      return PA_PRECEDENCE_FACTOR;
  }

  return PA_PRECEDENCE_NONE;
}

static void pa_parse(pa_t * t, i64 n);

static void pa_expression(pa_t * t) {
  pa_parse(t, PA_PRECEDENCE_OR);
}

static void pa_grouping(pa_t * t) {
  pa_advance(t);
  pa_expression(t);
  pa_consume(t, TK_RPAREN);
}

static void pa_unary(pa_t * t) {
  pa_advance(t);
  tk_t op = t->tok;
  pa_parse(t, PA_PRECEDENCE_UNARY);

  switch (op.tag) {
    case TK_NEG: break;
  }
}

static void pa_nud_rule_error(pa_t * t) {
  pa_maybe_report_error(t, t->lookahead.start, "expected expression");
}

static void pa_binary(pa_t * t) {
  pa_advance(t);
  tk_t op = t->tok;
  pa_parse(t, pa_precedence(op.tag) + 1);

  switch (op.tag) {
    case TK_ADD: break;
    case TK_SUB: break;
    case TK_MUL: break;
    case TK_DIV: break;
  }
}

static void pa_number(pa_t * t) {
  pa_advance(t);
}

static pa_nud_rule_t pa_nud_rule(tk_tag_t tag) {
  switch (tag) {
    case TK_LPAREN:
      return pa_grouping;
    case TK_NEG:
      return pa_unary;
    case TK_NUMBER:
      return pa_number;
  }

  return pa_nud_rule_error;
}

static pa_led_rule_t pa_led_rule(tk_tag_t tag) {
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

static void pa_parse(pa_t * t, pa_precedence_t precedence) {
  pa_nud_rule(t->lookahead.tag)(t);;

  while (precedence <= pa_precedence(t->lookahead.tag)) {
    pa_led_rule(t->lookahead.tag)(t);
  }
}
