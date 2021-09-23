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

static tk_t (* lx_next__jump[])(lx_t *, u8 *, u8 *);

static tk_t lx_next(lx_t * t) {
  u8 * p = t->next;
  tk_t r = lx_next__jump[* p](t, p, p + 1);
  t->next = r.stop;
  return r;
}

static tk_t lx_next__error(lx_t * t, u8 * p, u8 * q) {
  return tk_make(TK_ERROR, p, q);
}

static tk_t lx_next__eof(lx_t * t, u8 * p, u8 * q) {
  return tk_make(TK_EOF, p, q);
}

static tk_t lx_next__whitespace(lx_t * t, u8 * p, u8 * q) {
  u8 c = * q;

  while (c == '\t' || c == '\n' || c == ' ') {
    q ++;
    c = * q;
  }

  return lx_next__jump[c](t, q, q + 1);
}

static tk_t lx_next__comment(lx_t * t, u8 * p, u8 * q) {
  u8 c = * q;

  while (c != '\n') {
    q ++;
    c = * q;
  }

  q ++;
  c = * q;

  return lx_next__jump[c](t, q, q + 1);
}

static tk_t lx_next__id(lx_t * t, u8 * p, u8 * q) {
  u8 c = * q;

  while (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || (c == '_')) {
    q ++;
    c = * q;
  }

  // TODO: keywords

  return tk_make(TK_ID, p, q);
}

static tk_t lx_next__comma(lx_t * t, u8 * p, u8 * q) {
  return tk_make(TK_COMMA, p, q);
}

static tk_t lx_next__period(lx_t * t, u8 * p, u8 * q) {
  return tk_make(TK_PERIOD, p, q);
}

static tk_t lx_next__colon(lx_t * t, u8 * p, u8 * q) {
  return tk_make(TK_COLON, p, q);
}

static tk_t lx_next__semicolon(lx_t * t, u8 * p, u8 * q) {
  return tk_make(TK_SEMICOLON, p, q);
}

static tk_t lx_next__lbrace(lx_t * t, u8 * p, u8 * q) {
  return tk_make(TK_LBRACE, p, q);
}

static tk_t lx_next__rbrace(lx_t * t, u8 * p, u8 * q) {
  return tk_make(TK_RBRACE, p, q);
}

static tk_t lx_next__lbracket(lx_t * t, u8 * p, u8 * q) {
  return tk_make(TK_LBRACKET, p, q);
}

static tk_t lx_next__rbracket(lx_t * t, u8 * p, u8 * q) {
  return tk_make(TK_RBRACKET, p, q);
}

static tk_t lx_next__lparen(lx_t * t, u8 * p, u8 * q) {
  return tk_make(TK_LPAREN, p, q);
}

static tk_t lx_next__rparen(lx_t * t, u8 * p, u8 * q) {
  return tk_make(TK_RPAREN, p, q);
}

static tk_t (* lx_next__jump[])(lx_t *, u8 *, u8 *) = {
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
