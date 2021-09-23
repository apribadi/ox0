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

static lx_t lx_make(u8 * source) {
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

  q ++;
  c = * q;

  return lx_next__start(q, c);
}

static tk_t lx_next__id(u8 * p, u8 * q) {
  u8 c = * q;

  while (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || (c == '_')) {
    q ++;
    c = * q;
  }

  // TODO: keywords

  return tk_make(TK_ID, p, q);
}

static tk_t lx_next__comma(u8 * p, u8 * q) {
  return tk_make(TK_COMMA, p, q);
}

static tk_t lx_next__period(u8 * p, u8 * q) {
  return tk_make(TK_PERIOD, p, q);
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
  lx_next__error, // !
  lx_next__error, // "
  lx_next__comment, // #
  lx_next__error, // $
  lx_next__error, // %
  lx_next__error, // &
  lx_next__error, // '
  lx_next__lparen, // (
  lx_next__rparen, // )
  lx_next__error, // *
  lx_next__error, // +
  lx_next__comma, // ,
  lx_next__error, // -
  lx_next__period, // .
  lx_next__error, // /
  lx_next__error, // 0
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error,
  lx_next__error, // 9
  lx_next__colon, // :
  lx_next__semicolon, // ;
  lx_next__error, // <
  lx_next__error, // =
  lx_next__error, // >
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
  lx_next__error, // ~
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
