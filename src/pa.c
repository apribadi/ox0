// parsing

typedef struct {
  lx_t lex;
  tk_t tok;
  bool is_error;
  char * filename;
  char * source;
} pa_t;

enum {
  PA_BINDING_POWER_NONE,
  PA_BINDING_POWER_ASSIGNMENT,
  PA_BINDING_POWER_OR,         // right
  PA_BINDING_POWER_AND,        // right
  PA_BINDING_POWER_EQUALITY,   // left
  PA_BINDING_POWER_COMPARISON, // left
  PA_BINDING_POWER_TERM,       // left
  PA_BINDING_POWER_FACTOR,     // left
  PA_BINDING_POWER_PREFIX,
  PA_BINDING_POWER_CALL,
  PA_BINDING_POWER_PRIMARY,
};

typedef i64 pa_binding_power_t;

typedef void (* pa_null_rule_t) (pa_t *);

typedef void (* pa_left_rule_t) (pa_t *);

static pa_t pa_make(char * filename, char * source) {
  pa_t t;

  t.lex = lx_make(source);
  t.tok = lx_step(&t.lex);
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
  tk_t tok = lx_step(&t->lex);

  while (tok.tag == TK_ERROR) {
    pa_maybe_report_error(t, tok.start, "invalid token");
    tok = lx_step(&t->lex);
  }

  t->tok = tok;
}

static void pa_consume(pa_t * t, tk_tag_t tag) {
  if (t->tok.tag != tag) {
    pa_maybe_report_error(t, t->tok.start, "not expected token");
  };

  pa_advance(t);
}

static void pa_parse(pa_t * t, i64 n);

static void pa_expression(pa_t * t) {
  pa_parse(t, PA_BINDING_POWER_NONE);
}

static void pa_grouping(pa_t * t) {
  pa_advance(t);
  pa_expression(t);
  pa_consume(t, TK_RPAREN);
}

static void pa_neg(pa_t * t) {
  pa_advance(t);
  pa_parse(t, PA_BINDING_POWER_PREFIX);
}

static void pa_add(pa_t * t) {
  pa_advance(t);
  pa_parse(t, PA_BINDING_POWER_TERM);
}

static void pa_sub(pa_t * t) {
  pa_advance(t);
  pa_parse(t, PA_BINDING_POWER_TERM);
}

static void pa_mul(pa_t * t) {
  pa_advance(t);
  pa_parse(t, PA_BINDING_POWER_FACTOR);
}

static void pa_div(pa_t * t) {
  pa_advance(t);
  pa_parse(t, PA_BINDING_POWER_FACTOR);
}

static void pa_number(pa_t * t) {
  pa_advance(t);
}

static void pa_null_rule_error(pa_t * t) {
  pa_maybe_report_error(t, t->tok.start, "expected expression");
}

static pa_null_rule_t pa_null_rule(tk_tag_t tag) {
  switch (tag) {
    case TK_LPAREN:
      return pa_grouping;
    case TK_NEG:
      return pa_neg;
    case TK_NUMBER:
      return pa_number;
  }

  return pa_null_rule_error;
}

static pa_left_rule_t pa_left_rule(tk_tag_t tag) {
  switch (tag) {
    case TK_ADD:
      return pa_add;
    case TK_SUB:
      return pa_sub;
    case TK_MUL:
      return pa_mul;
    case TK_DIV:
      return pa_div;
  }

  return NULL;
}

static pa_binding_power_t pa_left_binding_power(tk_tag_t tag) {
  switch (tag) {
    case TK_ADD:
      return PA_BINDING_POWER_TERM;
    case TK_SUB:
      return PA_BINDING_POWER_TERM;
    case TK_MUL:
      return PA_BINDING_POWER_FACTOR;
    case TK_DIV:
      return PA_BINDING_POWER_FACTOR;
  }

  return PA_BINDING_POWER_NONE;
}

static void pa_parse(pa_t * t, pa_binding_power_t right_binding_power) {
  pa_null_rule(t->tok.tag)(t);;

  while (right_binding_power < pa_left_binding_power(t->tok.tag)) {
    pa_left_rule(t->tok.tag)(t);
  }
}
