// parsing

typedef struct {
  lx_t lex;
  tk_t tok;
  tk_t lookahead;
  bool is_panicking;
  char * filename;
  char * source;
} pa_t;

static pa_t pa_make(char * filename, char * source) {
  pa_t t;

  t.lex = lx_make(source);
  // t.tok = ??;
  t.lookahead = lx_step(&t.lex);
  t.is_panicking = false;
  t.filename = filename;
  t.source = source;

  return t;
}

static void pa_advance(pa_t * t) {
  t->tok = t->lookahead;
  t->lookahead = lx_step(&t->lex);
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
  char c = * q;

  while (c != '\n' && c != '\0') {
    ++ q;
    c = * q;
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
  if (!t->is_panicking) pa_report_error(t, location, message);
  t->is_panicking = true;
}

static void pa_consume(pa_t * t, tk_tag_t tag) {
  // TODO: real error handling

  if (t->lookahead.tag != tag)
    panic("pa_consume: tag mismatch!");

  pa_advance(t);
}

enum {
  PA_PRECEDENCE_NONE,
  PA_PRECEDENCE_ASSIGNMENT,
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
  pa_parse(t, PA_PRECEDENCE_ASSIGNMENT);
}

static void pa_grouping(pa_t * t) {
  pa_expression(t);
  pa_consume(t, TK_RPAREN);
}

static void pa_unary(pa_t * t) {
  tk_t op = t->tok;

  pa_parse(t, PA_PRECEDENCE_UNARY);

  switch (op.tag) {
    case TK_NEG: break;
  }
}

static void pa_binary(pa_t * t) {
  tk_t op = t->tok;

  pa_precedence_t precedence = pa_precedence(op.tag);

  pa_parse(t, precedence + 1);

  switch (op.tag) {
    case TK_ADD: break;
    case TK_SUB: break;
    case TK_MUL: break;
    case TK_DIV: break;
  }
}

static void pa_number(pa_t * t) {
  (void) t;
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

static void pa_parse(pa_t * t, pa_precedence_t precedence) {
  pa_advance(t);

  pa_rule_t prefix_rule = pa_prefix_rule(t->tok.tag);

  if (!prefix_rule) {
    pa_maybe_report_error(t, t->tok.start, "expected expression");
    panic("parse error!");
    return;
  }

  prefix_rule(t);

  while (precedence <= pa_precedence(t->lookahead.tag)) {
    pa_advance(t);
    pa_rule_t infix_rule = pa_infix_rule(t->tok.tag);
    infix_rule(t);
  }
}
