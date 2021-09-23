// lexer
//
// lx_t
//
// lx_t lx_make(u8 * source)
//
// tk_t lx_lex(lx_t * t)

typedef struct {
  u8 * next;
} lx_t;

// Source MUST be terminated by a '\0' character.

static lx_t lx_make(u8 * source) {
  return (lx_t) { .next = source };
}

static tk_t (* lx_lex__jump[])(lx_t *, u8 *, u8 *);

static tk_t lx_lex(lx_t * t) {
  u8 * p = t->next;
  tk_t r = lx_lex__jump[* p](t, p, p + 1);
  t->next = r.stop;
  return r;
}

static tk_t lx_lex__error(lx_t * t, u8 * p, u8 * q) {
  return tk_make(TK_ERROR, p, q);
}

static tk_t lx_lex__eof(lx_t * t, u8 * p, u8 * q) {
  return tk_make(TK_EOF, p, q);
}

static tk_t lx_lex__whitespace(lx_t * t, u8 * p, u8 * q) {
  u8 c = * q;

  while (c == '\t' || c == '\n' || c == ' ') {
    q ++;
    c = * q;
  }

  return lx_lex__jump[c](t, q, q + 1);
}

static tk_t lx_lex__comment(lx_t * t, u8 * p, u8 * q) {
  u8 c = * q;

  while (c != '\n') {
    q ++;
    c = * q;
  }

  return lx_lex__jump[c](t, q, q + 1);
}

static tk_t lx_lex__comma(lx_t * t, u8 * p, u8 * q) {
  return tk_make(TK_COMMA, p, q);
}

static tk_t lx_lex__period(lx_t * t, u8 * p, u8 * q) {
  return tk_make(TK_PERIOD, p, q);
}

static tk_t lx_lex__colon(lx_t * t, u8 * p, u8 * q) {
  return tk_make(TK_COLON, p, q);
}

static tk_t lx_lex__semicolon(lx_t * t, u8 * p, u8 * q) {
  return tk_make(TK_SEMICOLON, p, q);
}

static tk_t lx_lex__lbrace(lx_t * t, u8 * p, u8 * q) {
  return tk_make(TK_LBRACE, p, q);
}

static tk_t lx_lex__rbrace(lx_t * t, u8 * p, u8 * q) {
  return tk_make(TK_RBRACE, p, q);
}

static tk_t lx_lex__lbracket(lx_t * t, u8 * p, u8 * q) {
  return tk_make(TK_LBRACKET, p, q);
}

static tk_t lx_lex__rbracket(lx_t * t, u8 * p, u8 * q) {
  return tk_make(TK_RBRACKET, p, q);
}

static tk_t lx_lex__lparen(lx_t * t, u8 * p, u8 * q) {
  return tk_make(TK_LPAREN, p, q);
}

static tk_t lx_lex__rparen(lx_t * t, u8 * p, u8 * q) {
  return tk_make(TK_RPAREN, p, q);
}

static tk_t (* lx_lex__jump[])(lx_t *, u8 *, u8 *) = {
  lx_lex__eof, // \0
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__whitespace, // \t
  lx_lex__whitespace, // \n
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__whitespace, // ' '
  lx_lex__error, // !
  lx_lex__error, // "
  lx_lex__comment, // #
  lx_lex__error, // $
  lx_lex__error, // %
  lx_lex__error, // &
  lx_lex__error, // '
  lx_lex__lparen, // (
  lx_lex__rparen, // )
  lx_lex__error, // *
  lx_lex__error, // +
  lx_lex__comma, // ,
  lx_lex__error, // -
  lx_lex__period, // .
  lx_lex__error, // /
  lx_lex__error, // 0
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error, // 9
  lx_lex__colon, // :
  lx_lex__semicolon, // ;
  lx_lex__error, // <
  lx_lex__error, // =
  lx_lex__error, // >
  lx_lex__error, // ?
  lx_lex__error, // @
  lx_lex__error, // A
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error, // Z
  lx_lex__lbracket, // [
  lx_lex__error, // '\'
  lx_lex__rbracket, // ]
  lx_lex__error, // ^
  lx_lex__error, // _
  lx_lex__error, // `
  lx_lex__error, // a
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error, // z
  lx_lex__lbrace, // {
  lx_lex__error, // |
  lx_lex__rbrace, // }
  lx_lex__error, // ~
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
  lx_lex__error,
};
