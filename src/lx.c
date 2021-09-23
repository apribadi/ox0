// lexer
//
// lx_t
//
// lx_t lx_make(u8 * source)
//
// tk_t lx_next(lx_t * t)

typedef struct {
  u8 * next;
} lx_t;

// Source MUST be terminated by a '\0' character.

static inline lx_t lx_make(u8 * source) {
  return (lx_t) { .next = source };
}

static tk_t (* lx_next__jump[])(u8 *, u8 *);

static inline tk_t lx_next__start(u8 * p, u8 c) {
  return lx_next__jump[c](p, p + 1);
}

static inline tk_t lx_next(lx_t * t) {
  u8 * p = t->next;
  tk_t r = lx_next__start(p, * p);
  t->next = r.stop;
  return r;
}

static tk_t lx_next__error(u8 * p, u8 * q) {
  return tk_make(TK_ERROR, p, q);
}

static tk_t lx_next__eof(u8 * p, u8 * q) {
  return tk_make(TK_EOF, p, q);
}

static tk_t lx_next__whitespace(u8 * p, u8 * q) {
  (void) p;

  u8 c = * q;

  while (c == '\t' || c == '\n' || c == ' ') {
    q ++;
    c = * q;
  }

  return lx_next__start(q, c);
}

static tk_t lx_next__comment(u8 * p, u8 * q) {
  (void) p;

  u8 c = * q;

  while (c != '\n') {
    q ++;
    c = * q;
  }

  // We include the line break in the comment token.

  q ++;
  c = * q;

  return lx_next__start(q, c);
}

static tk_t lx_next__id(u8 * p, u8 * q) {
  u8 c = * q;

  while (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9') || (c == '_')) {
    q ++;
    c = * q;
  }

  // TODO: keywords

  return tk_make(TK_ID, p, q);
}

static tk_t lx_next__num(u8 * p, u8 * q) {
  u8 c = * q;

  while ('0' <= c && c <= '9') {
    q ++;
    c = * q;
  }

  return tk_make(TK_NUM, p, q);
}

static tk_t lx_next__comma(u8 * p, u8 * q) {
  return tk_make(TK_COMMA, p, q);
}

static tk_t lx_next__dot(u8 * p, u8 * q) {
  return tk_make(TK_DOT, p, q);
}

static tk_t lx_next__colon(u8 * p, u8 * q) {
  return tk_make(TK_COLON, p, q);
}

static tk_t lx_next__semicolon(u8 * p, u8 * q) {
  return tk_make(TK_SEMICOLON, p, q);
}

static tk_t lx_next__lbrace(u8 * p, u8 * q) {
  return tk_make(TK_LBRACE, p, q);
}

static tk_t lx_next__rbrace(u8 * p, u8 * q) {
  return tk_make(TK_RBRACE, p, q);
}

static tk_t lx_next__lbracket(u8 * p, u8 * q) {
  return tk_make(TK_LBRACKET, p, q);
}

static tk_t lx_next__rbracket(u8 * p, u8 * q) {
  return tk_make(TK_RBRACKET, p, q);
}

static tk_t lx_next__lparen(u8 * p, u8 * q) {
  return tk_make(TK_LPAREN, p, q);
}

static tk_t lx_next__rparen(u8 * p, u8 * q) {
  return tk_make(TK_RPAREN, p, q);
}

static tk_t lx_next__plus(u8 * p, u8 * q) {
  return tk_make(TK_ADD, p, q);
}

static tk_t lx_next__dash(u8 * p, u8 * q) {
  u8 c = * q;

  if ('0' <= c && c <= '9') {
    return lx_next__num(p, q + 1);
  }

  return tk_make(TK_SUB, p, q);
}

static tk_t lx_next__star(u8 * p, u8 * q) {
  return tk_make(TK_MUL, p, q);
}

static tk_t lx_next__slash(u8 * p, u8 * q) {
  return tk_make(TK_DIV, p, q);
}

static tk_t lx_next__tilde(u8 * p, u8 * q) {
  return tk_make(TK_NEG, p, q);
}

static tk_t lx_next__equal(u8 * p, u8 * q) {
  u8 c = * q;

  if (c == '=') {
    return tk_make(TK_EQ, p, q + 1);
  }

  return tk_make(TK_LET_EQUAL, p, q);
}

static tk_t lx_next__bang(u8 * p, u8 * q) {
  u8 c = * q;

  if (c == '=') {
    return tk_make(TK_NE, p, q + 1);
  }

  return tk_make(TK_ERROR, p, q);
}

static tk_t lx_next__langle(u8 * p, u8 * q) {
  u8 c = * q;

  if (c == '=') {
    return tk_make(TK_LE, p, q + 1);
  }

  return tk_make(TK_LT, p, q);
}

static tk_t lx_next__rangle(u8 * p, u8 * q) {
  u8 c = * q;

  if (c == '=') {
    return tk_make(TK_GE, p, q + 1);
  }

  return tk_make(TK_GT, p, q);
}

static tk_t (* lx_next__jump[])(u8 *, u8 *) = {
  lx_next__eof, // \0
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__whitespace, // \t
  lx_next__whitespace, // \n
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__whitespace, // ' '
  lx_next__bang, // !
  lx_next__error, // "
  lx_next__comment, // #
  lx_next__error, // $
  lx_next__error, // %
  lx_next__error, // &
  lx_next__error, // '
  lx_next__lparen, // (
  lx_next__rparen, // )
  lx_next__star, // *
  lx_next__plus, // +
  lx_next__comma, // ,
  lx_next__dash, // -
  lx_next__dot, // .
  lx_next__slash, // /
  lx_next__num, // 0
  lx_next__num,
  lx_next__num,
  lx_next__num,
  lx_next__num,
  lx_next__num,
  lx_next__num,
  lx_next__num,
  lx_next__num,
  lx_next__num, // 9
  lx_next__colon, // :
  lx_next__semicolon, // ;
  lx_next__langle, // <
  lx_next__equal, // =
  lx_next__rangle, // >
  lx_next__error, // ?
  lx_next__error, // @
  lx_next__id, // A
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id, // Z
  lx_next__lbracket, // [
  lx_next__error, // '\'
  lx_next__rbracket, // ]
  lx_next__error, // ^
  lx_next__error, // _
  lx_next__error, // `
  lx_next__id, // a
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id,
  lx_next__id, // z
  lx_next__lbrace, // {
  lx_next__error, // |
  lx_next__rbrace, // }
  lx_next__tilde, // ~
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
};
