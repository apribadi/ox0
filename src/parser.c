// parsing

typedef struct {
  Lexer lexer;
  Token token;
  Arena * arena;
  bool is_panicking;
  char const * filename;
  char const * source;
} Parser;

static Parser parser_make(Arena * arena, char const * filename, char const * source) {
  Parser t;

  t.lexer = lexer_make(source);
  t.token = lexer_next(&t.lexer);
  t.arena = arena;
  t.is_panicking = false;
  t.filename = filename;
  t.source = source;

  return t;
}

static void parser_report_error(Parser * t, char const * location, char const * message) {
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

static void parser_maybe_report_error(Parser * t, char const * location, char const * message) {
  if (t->is_panicking) return;
  t->is_panicking = true;
  parser_report_error(t, location, message);
}

static void parser_advance(Parser * t) {
  Token tok = lexer_next(&t->lexer);

  while (tok.tag == TOKEN_ILLEGAL_CHARACTER) {
    parser_maybe_report_error(t, tok.start, "illegal character");
    tok = lexer_next(&t->lexer);
  }

  t->token = tok;
}

static void parser_consume(Parser * t, TokenTag tag) {
  if (t->token.tag != tag) {
    parser_maybe_report_error(t, t->token.start, "not expected token");
  };

  parser_advance(t);
}

typedef enum : u8 {
  PARSER_EXPRESSION_BINDING_POWER_NONE,
  PARSER_EXPRESSION_BINDING_POWER_ASSIGNMENT,
  PARSER_EXPRESSION_BINDING_POWER_OR,         // right
  PARSER_EXPRESSION_BINDING_POWER_AND,        // right
  PARSER_EXPRESSION_BINDING_POWER_EQUALITY,   // left
  PARSER_EXPRESSION_BINDING_POWER_COMPARISON, // left
  PARSER_EXPRESSION_BINDING_POWER_TERM,       // left
  PARSER_EXPRESSION_BINDING_POWER_FACTOR,     // left
  PARSER_EXPRESSION_BINDING_POWER_PREFIX,
  PARSER_EXPRESSION_BINDING_POWER_CALL,
  PARSER_EXPRESSION_BINDING_POWER_PRIMARY,
} ParserBindingPower;

typedef ParserResult (* ParserExpressionNullRule) (Parser *);

typedef ParserResult (* ParserExpressionLeftRule) (Parser *, ParserResult);

static ParserResult parser_expression_with_precedence(Parser * t, ParserBindingPower n);

static ParserResult parser_expression(Parser * t) {
  return parser_expression_with_precedence(t, PARSER_EXPRESSION_BINDING_POWER_NONE);
}

static ParserResult parser_expression_grouping(Parser * t) {
  parser_advance(t);
  ParserResult a = parser_expression(t);
  parser_consume(t, TOKEN_RPAREN);

  return a;
}

static ParserResult parser_neg(Parser * t) {
  parser_advance(t);
  ParserResult a = parser_expression_with_precedence(t, PARSER_EXPRESSION_BINDING_POWER_PREFIX);

  Sexp e = sexp_make_list(t->arena, 2);
  e.as.list.data[0] = sexp_make_atom(1, "-");
  e.as.list.data[1] = a;

  return e;
}

static ParserResult parser_add(Parser * t, ParserResult a) {
  parser_advance(t);
  ParserResult b = parser_expression_with_precedence(t, PARSER_EXPRESSION_BINDING_POWER_TERM);

  Sexp e = sexp_make_list(t->arena, 3);
  e.as.list.data[0] = sexp_make_atom(1, "+");
  e.as.list.data[1] = a;
  e.as.list.data[2] = b;

  return e;
}

static ParserResult parser_sub(Parser * t, ParserResult a) {
  parser_advance(t);
  ParserResult b = parser_expression_with_precedence(t, PARSER_EXPRESSION_BINDING_POWER_TERM);

  Sexp e = sexp_make_list(t->arena, 3);
  e.as.list.data[0] = sexp_make_atom(1, "-");
  e.as.list.data[1] = a;
  e.as.list.data[2] = b;

  return e;
}

static ParserResult parser_mul(Parser * t, ParserResult a) {
  parser_advance(t);
  ParserResult b = parser_expression_with_precedence(t, PARSER_EXPRESSION_BINDING_POWER_FACTOR);

  Sexp e = sexp_make_list(t->arena, 3);
  e.as.list.data[0] = sexp_make_atom(1, "*");
  e.as.list.data[1] = a;
  e.as.list.data[2] = b;

  return e;
}

static ParserResult parser_div(Parser * t, ParserResult a) {
  parser_advance(t);
  ParserResult b = parser_expression_with_precedence(t, PARSER_EXPRESSION_BINDING_POWER_FACTOR);

  Sexp e = sexp_make_list(t->arena, 3);
  e.as.list.data[0] = sexp_make_atom(1, "/");
  e.as.list.data[1] = a;
  e.as.list.data[2] = b;

  return e;
}

static ParserResult parser_number(Parser * t) {
  Sexp e = sexp_make_atom(t->token.stop - t->token.start, t->token.start);

  parser_advance(t);

  return e;
}

static ParserResult parser_null_rule_error(Parser * t) {
  parser_maybe_report_error(t, t->token.start, "expected expression");

  return sexp_make_atom(5, "ERROR");
}

static ParserExpressionNullRule parser_null_rule(TokenTag tag) {
  switch (tag) {
    case TOKEN_LPAREN:
      return parser_expression_grouping;
    case TOKEN_NEG:
      return parser_neg;
    case TOKEN_NUMBER:
      return parser_number;
  }

  return parser_null_rule_error;
}

static ParserExpressionLeftRule parser_left_rule(TokenTag tag) {
  switch (tag) {
    case TOKEN_ADD:
      return parser_add;
    case TOKEN_SUB:
      return parser_sub;
    case TOKEN_MUL:
      return parser_mul;
    case TOKEN_DIV:
      return parser_div;
  }

  return NULL;
}

static ParserBindingPower parser_left_binding_power(TokenTag tag) {
  switch (tag) {
    case TOKEN_ADD:
      return PARSER_EXPRESSION_BINDING_POWER_TERM;
    case TOKEN_SUB:
      return PARSER_EXPRESSION_BINDING_POWER_TERM;
    case TOKEN_MUL:
      return PARSER_EXPRESSION_BINDING_POWER_FACTOR;
    case TOKEN_DIV:
      return PARSER_EXPRESSION_BINDING_POWER_FACTOR;
  }

  return PARSER_EXPRESSION_BINDING_POWER_NONE;
}

static ParserResult parser_expression_with_precedence(Parser * t, ParserBindingPower right_binding_power) {
  ParserResult e = parser_null_rule(t->token.tag)(t);;

  while (right_binding_power < parser_left_binding_power(t->token.tag)) {
    e = parser_left_rule(t->token.tag)(t, e);
  }

  return e;
}
