// parser

typedef struct {
  lx_t lexer;
  lx_token_t token;
  mm_arena_t * arena;
  bool is_panicking;
  char const * filename;
  char const * source;
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

typedef sx_t pa_result_t;

typedef pa_result_t (* pa_null_rule_t) (pa_t *);

typedef pa_result_t (* pa_left_rule_t) (pa_t *, pa_result_t);

static pa_t pa_make(mm_arena_t * arena, char const * filename, char const * source) {
  pa_t t;

  t.lexer = lx_make(source);
  t.token = lx_step(&t.lexer);
  t.arena = arena;
  t.is_panicking = false;
  t.filename = filename;
  t.source = source;

  return t;
}

static void pa_report_error(pa_t * t, char const * location, char const * message) {
  i64 line_number = 1;
  char const * line_start = t->source;
  char const * p = t->source;

  while (p != location) {
    if (* p == '\n') {
      line_number = line_number + 1;
      line_start = p + 1;
    }

    p ++;
  }

  char const * q = line_start;

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

static void pa_maybe_report_error(pa_t * t, char const * location, char const * message) {
  if (t->is_panicking) return;
  t->is_panicking = true;
  pa_report_error(t, location, message);
}

static void pa_advance(pa_t * t) {
  lx_token_t token = lx_step(&t->lexer);

  while (token.tag == LX_TOKEN_ERROR) {
    pa_maybe_report_error(t, token.start, "invalid token");
    token = lx_step(&t->lexer);
  }

  t->token = token;
}

static void pa_consume(pa_t * t, lx_token_tag_t tag) {
  if (t->token.tag != tag) {
    pa_maybe_report_error(t, t->token.start, "not expected token");
  };

  pa_advance(t);
}

static pa_result_t pa_parse(pa_t * t, i64 n);

static pa_result_t pa_expression(pa_t * t) {
  return pa_parse(t, PA_BINDING_POWER_NONE);
}

static pa_result_t pa_grouping(pa_t * t) {
  pa_advance(t);
  pa_result_t a = pa_expression(t);
  pa_consume(t, LX_TOKEN_RPAREN);

  return a;
}

static pa_result_t pa_neg(pa_t * t) {
  pa_advance(t);
  pa_result_t a = pa_parse(t, PA_BINDING_POWER_PREFIX);

  sx_t e = sx_make_list(t->arena, 2);
  e.as.list.data[0] = sx_make_atom(1, "-");
  e.as.list.data[1] = a;

  return e;
}

static pa_result_t pa_add(pa_t * t, pa_result_t a) {
  pa_advance(t);
  pa_result_t b = pa_parse(t, PA_BINDING_POWER_TERM);

  sx_t e = sx_make_list(t->arena, 3);
  e.as.list.data[0] = sx_make_atom(1, "+");
  e.as.list.data[1] = a;
  e.as.list.data[2] = b;

  return e;
}

static pa_result_t pa_sub(pa_t * t, pa_result_t a) {
  pa_advance(t);
  pa_result_t b = pa_parse(t, PA_BINDING_POWER_TERM);

  sx_t e = sx_make_list(t->arena, 3);
  e.as.list.data[0] = sx_make_atom(1, "-");
  e.as.list.data[1] = a;
  e.as.list.data[2] = b;

  return e;
}

static pa_result_t pa_mul(pa_t * t, pa_result_t a) {
  pa_advance(t);
  pa_result_t b = pa_parse(t, PA_BINDING_POWER_FACTOR);

  sx_t e = sx_make_list(t->arena, 3);
  e.as.list.data[0] = sx_make_atom(1, "*");
  e.as.list.data[1] = a;
  e.as.list.data[2] = b;

  return e;
}

static pa_result_t pa_div(pa_t * t, pa_result_t a) {
  pa_advance(t);
  pa_result_t b = pa_parse(t, PA_BINDING_POWER_FACTOR);

  sx_t e = sx_make_list(t->arena, 3);
  e.as.list.data[0] = sx_make_atom(1, "/");
  e.as.list.data[1] = a;
  e.as.list.data[2] = b;

  return e;
}

static pa_result_t pa_number(pa_t * t) {
  sx_t e = sx_make_atom(t->token.stop - t->token.start, t->token.start);

  pa_advance(t);

  return e;
}

static pa_result_t pa_null_rule_error(pa_t * t) {
  pa_maybe_report_error(t, t->token.start, "expected expression");

  return sx_make_atom(5, "ERROR");
}

static pa_null_rule_t pa_null_rule(lx_token_tag_t tag) {
  switch (tag) {
    case LX_TOKEN_LPAREN:
      return pa_grouping;
    case LX_TOKEN_NEG:
      return pa_neg;
    case LX_TOKEN_NUMBER:
      return pa_number;
  }

  return pa_null_rule_error;
}

static pa_left_rule_t pa_left_rule(lx_token_tag_t tag) {
  switch (tag) {
    case LX_TOKEN_ADD:
      return pa_add;
    case LX_TOKEN_SUB:
      return pa_sub;
    case LX_TOKEN_MUL:
      return pa_mul;
    case LX_TOKEN_DIV:
      return pa_div;
  }

  return NULL;
}

static pa_binding_power_t pa_left_binding_power(lx_token_tag_t tag) {
  switch (tag) {
    case LX_TOKEN_ADD:
      return PA_BINDING_POWER_TERM;
    case LX_TOKEN_SUB:
      return PA_BINDING_POWER_TERM;
    case LX_TOKEN_MUL:
      return PA_BINDING_POWER_FACTOR;
    case LX_TOKEN_DIV:
      return PA_BINDING_POWER_FACTOR;
  }

  return PA_BINDING_POWER_NONE;
}

static pa_result_t pa_parse(pa_t * t, pa_binding_power_t right_binding_power) {
  pa_result_t e;

  e = pa_null_rule(t->token.tag)(t);;

  while (right_binding_power < pa_left_binding_power(t->token.tag)) {
    e = pa_left_rule(t->token.tag)(t, e);
  }

  return e;
}
