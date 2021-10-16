// parsing

typedef struct {
  Lex lex;
  Token token;
  mm_arena_t * arena;
  bool is_panicking;
  char const * filename;
  char const * source;
} Parse;

typedef enum : u8 {
  PARSE_BINDING_POWER_NONE,
  PARSE_BINDING_POWER_ASSIGNMENT,
  PARSE_BINDING_POWER_OR,         // right
  PARSE_BINDING_POWER_AND,        // right
  PARSE_BINDING_POWER_EQUALITY,   // left
  PARSE_BINDING_POWER_COMPARISON, // left
  PARSE_BINDING_POWER_TERM,       // left
  PARSE_BINDING_POWER_FACTOR,     // left
  PARSE_BINDING_POWER_PREFIX,
  PARSE_BINDING_POWER_CALL,
  PARSE_BINDING_POWER_PRIMARY,
} ParseBindingPower;

typedef Sexp ParseResult;

typedef ParseResult (* ParseNullRule) (Parse *);

typedef ParseResult (* ParseLeftRule) (Parse *, ParseResult);

static Parse parse_make(mm_arena_t * arena, char const * filename, char const * source) {
  Parse t;

  t.lex = lex_make(source);
  t.token = lex_next(&t.lex);
  t.arena = arena;
  t.is_panicking = false;
  t.filename = filename;
  t.source = source;

  return t;
}

static void parse_report_error(Parse * t, char const * location, char const * message) {
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
}

static void parse_maybe_report_error(Parse * t, char const * location, char const * message) {
  if (t->is_panicking) return;
  t->is_panicking = true;
  parse_report_error(t, location, message);
}

static void parse_advance(Parse * t) {
  Token tok = lex_next(&t->lex);

  while (tok.tag == TOKEN_ILLEGAL_CHARACTER) {
    parse_maybe_report_error(t, tok.start, "illegal character");
    tok = lex_next(&t->lex);
  }

  t->token = tok;
}

static void parse_consume(Parse * t, TokenTag tag) {
  if (t->token.tag != tag) {
    parse_maybe_report_error(t, t->token.start, "not expected token");
  };

  parse_advance(t);
}

static ParseResult parse_precedence(Parse * t, ParseBindingPower n);

static ParseResult parse_expression(Parse * t) {
  return parse_precedence(t, PARSE_BINDING_POWER_NONE);
}

static ParseResult parse_grouping(Parse * t) {
  parse_advance(t);
  ParseResult a = parse_expression(t);
  parse_consume(t, TOKEN_RPAREN);

  return a;
}

static ParseResult parse_neg(Parse * t) {
  parse_advance(t);
  ParseResult a = parse_precedence(t, PARSE_BINDING_POWER_PREFIX);

  Sexp e = sexp_make_list(t->arena, 2);
  e.as.list.data[0] = sexp_make_atom(1, "-");
  e.as.list.data[1] = a;

  return e;
}

static ParseResult parse_add(Parse * t, ParseResult a) {
  parse_advance(t);
  ParseResult b = parse_precedence(t, PARSE_BINDING_POWER_TERM);

  Sexp e = sexp_make_list(t->arena, 3);
  e.as.list.data[0] = sexp_make_atom(1, "+");
  e.as.list.data[1] = a;
  e.as.list.data[2] = b;

  return e;
}

static ParseResult parse_sub(Parse * t, ParseResult a) {
  parse_advance(t);
  ParseResult b = parse_precedence(t, PARSE_BINDING_POWER_TERM);

  Sexp e = sexp_make_list(t->arena, 3);
  e.as.list.data[0] = sexp_make_atom(1, "-");
  e.as.list.data[1] = a;
  e.as.list.data[2] = b;

  return e;
}

static ParseResult parse_mul(Parse * t, ParseResult a) {
  parse_advance(t);
  ParseResult b = parse_precedence(t, PARSE_BINDING_POWER_FACTOR);

  Sexp e = sexp_make_list(t->arena, 3);
  e.as.list.data[0] = sexp_make_atom(1, "*");
  e.as.list.data[1] = a;
  e.as.list.data[2] = b;

  return e;
}

static ParseResult parse_div(Parse * t, ParseResult a) {
  parse_advance(t);
  ParseResult b = parse_precedence(t, PARSE_BINDING_POWER_FACTOR);

  Sexp e = sexp_make_list(t->arena, 3);
  e.as.list.data[0] = sexp_make_atom(1, "/");
  e.as.list.data[1] = a;
  e.as.list.data[2] = b;

  return e;
}

static ParseResult parse_number(Parse * t) {
  Sexp e = sexp_make_atom(t->token.stop - t->token.start, t->token.start);

  parse_advance(t);

  return e;
}

static ParseResult parse_null_rule_error(Parse * t) {
  parse_maybe_report_error(t, t->token.start, "expected expression");

  return sexp_make_atom(5, "ERROR");
}

static ParseNullRule parse_null_rule(TokenTag tag) {
  switch (tag) {
    case TOKEN_LPAREN:
      return parse_grouping;
    case TOKEN_NEG:
      return parse_neg;
    case TOKEN_NUMBER:
      return parse_number;
  }

  return parse_null_rule_error;
}

static ParseLeftRule parse_left_rule(TokenTag tag) {
  switch (tag) {
    case TOKEN_ADD:
      return parse_add;
    case TOKEN_SUB:
      return parse_sub;
    case TOKEN_MUL:
      return parse_mul;
    case TOKEN_DIV:
      return parse_div;
  }

  return NULL;
}

static ParseBindingPower parse_left_binding_power(TokenTag tag) {
  switch (tag) {
    case TOKEN_ADD:
      return PARSE_BINDING_POWER_TERM;
    case TOKEN_SUB:
      return PARSE_BINDING_POWER_TERM;
    case TOKEN_MUL:
      return PARSE_BINDING_POWER_FACTOR;
    case TOKEN_DIV:
      return PARSE_BINDING_POWER_FACTOR;
  }

  return PARSE_BINDING_POWER_NONE;
}

static ParseResult parse_precedence(Parse * t, ParseBindingPower right_binding_power) {
  ParseResult e = parse_null_rule(t->token.tag)(t);;

  while (right_binding_power < parse_left_binding_power(t->token.tag)) {
    e = parse_left_rule(t->token.tag)(t, e);
  }

  return e;
}
