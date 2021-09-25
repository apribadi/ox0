// lexing
//
// lx_t
//
// lx_t lx_make(char * source)
//
// tk_t lx_next(lx_t * t)

typedef struct {
  char * next;
} lx_t;

// Source MUST be terminated by a '\0' character.

static inline lx_t lx_make(char * source) {
  lx_t t;

  t.next = source;

  return t;
}

static tk_t (* lx_next__jump[])(char *, char *);

static inline tk_t lx_next__start(char * p, char c) {
  return lx_next__jump[(u8) c](p, p + 1);
}

static inline tk_t lx_next(lx_t * t) {
  char * p = t->next;
  tk_t r = lx_next__start(p, * p);
  t->next = r.stop;
  return r;
}

static inline bool lx_is_alpha(char c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

static inline bool lx_is_digit(char c) {
  return ('0' <= c && c <= '9');
}

static inline bool lx_is_keyword(char * p, char * q, i64 m, i64 n, char * s) {
  return (p + m + n == q) && ! bcmp(p + m, s, n);
}

static tk_t lx_next__error(char * p, char * q) {
  return tk_make(TK_ERROR, p, q);
}

static tk_t lx_next__eof(char * p, char * q) {
  return tk_make(TK_EOF, p, q);
}

static tk_t lx_next__whitespace(__attribute__((unused)) char * p, char * q) {
  char c = * q;

  while (c == '\t' || c == '\n' || c == ' ') {
    q ++;
    c = * q;
  }

  return lx_next__start(q, c);
}

static tk_t lx_next__comment(__attribute__((unused)) char * p, char * q) {
  char c = * q;

  while (c != '\n') {
    q ++;
    c = * q;
  }

  // We include the line break in the comment token.

  q ++;
  c = * q;

  return lx_next__start(q, c);
}

static tk_t lx_next__id(char * p, char * q) {
  char c = * q;

  while (lx_is_alpha(c) || lx_is_digit(c) || c == '_') {
    q ++;
    c = * q;
  }

  return tk_make(TK_ID, p, q);
}

static tk_t lx_next__id_c(char * p, char * q) {
  char c = * q;

  while (lx_is_alpha(c) || lx_is_digit(c) || c == '_') {
    q ++;
    c = * q;
  }

  if (lx_is_keyword(p, q, 1, 3, "ase")) return tk_make(TK_CASE, p, q);

  return tk_make(TK_ID, p, q);
}

static tk_t lx_next__id_d(char * p, char * q) {
  char c = * q;

  while (lx_is_alpha(c) || lx_is_digit(c) || c == '_') {
    q ++;
    c = * q;
  }

  if (lx_is_keyword(p, q, 1, 1, "o")) return tk_make(TK_DO, p, q);

  return tk_make(TK_ID, p, q);
}

static tk_t lx_next__id_e(char * p, char * q) {
  char c = * q;

  while (lx_is_alpha(c) || lx_is_digit(c) || c == '_') {
    q ++;
    c = * q;
  }

  if (lx_is_keyword(p, q, 1, 3, "lif")) return tk_make(TK_ELIF, p, q);
  if (lx_is_keyword(p, q, 1, 3, "lse")) return tk_make(TK_ELSE, p, q);
  if (lx_is_keyword(p, q, 1, 2, "nd")) return tk_make(TK_END, p, q);

  return tk_make(TK_ID, p, q);
}

static tk_t lx_next__id_f(char * p, char * q) {
  char c = * q;

  while (lx_is_alpha(c) || lx_is_digit(c) || c == '_') {
    q ++;
    c = * q;
  }

  if (lx_is_keyword(p, q, 1, 2, "or")) return tk_make(TK_FOR, p, q);
  if (lx_is_keyword(p, q, 1, 2, "un")) return tk_make(TK_FUN, p, q);

  return tk_make(TK_ID, p, q);
}

static tk_t lx_next__id_i(char * p, char * q) {
  char c = * q;

  while (lx_is_alpha(c) || lx_is_digit(c) || c == '_') {
    q ++;
    c = * q;
  }

  if (lx_is_keyword(p, q, 1, 1, "f")) return tk_make(TK_IF, p, q);

  return tk_make(TK_ID, p, q);
}

static tk_t lx_next__id_l(char * p, char * q) {
  char c = * q;

  while (lx_is_alpha(c) || lx_is_digit(c) || c == '_') {
    q ++;
    c = * q;
  }

  if (lx_is_keyword(p, q, 1, 2, "et")) return tk_make(TK_LET, p, q);

  return tk_make(TK_ID, p, q);
}

static tk_t lx_next__id_t(char * p, char * q) {
  char c = * q;

  while (lx_is_alpha(c) || lx_is_digit(c) || c == '_') {
    q ++;
    c = * q;
  }

  if (lx_is_keyword(p, q, 1, 3, "hen")) return tk_make(TK_THEN, p, q);

  return tk_make(TK_ID, p, q);
}

static tk_t lx_next__id_w(char * p, char * q) {
  char c = * q;

  while (lx_is_alpha(c) || lx_is_digit(c) || c == '_') {
    q ++;
    c = * q;
  }

  if (lx_is_keyword(p, q, 1, 4, "hile")) return tk_make(TK_WHILE, p, q);

  return tk_make(TK_ID, p, q);
}

static tk_t lx_next__number(char * p, char * q) {
  char c = * q;

  while (lx_is_digit(c)) {
    q ++;
    c = * q;
  }

  return tk_make(TK_NUMBER, p, q);
}

static tk_t lx_next__comma(char * p, char * q) {
  return tk_make(TK_COMMA, p, q);
}

static tk_t lx_next__dot(char * p, char * q) {
  return tk_make(TK_DOT, p, q);
}

static tk_t lx_next__colon(char * p, char * q) {
  return tk_make(TK_COLON, p, q);
}

static tk_t lx_next__semicolon(char * p, char * q) {
  return tk_make(TK_SEMICOLON, p, q);
}

static tk_t lx_next__lbrace(char * p, char * q) {
  return tk_make(TK_LBRACE, p, q);
}

static tk_t lx_next__rbrace(char * p, char * q) {
  return tk_make(TK_RBRACE, p, q);
}

static tk_t lx_next__lbracket(char * p, char * q) {
  return tk_make(TK_LBRACKET, p, q);
}

static tk_t lx_next__rbracket(char * p, char * q) {
  return tk_make(TK_RBRACKET, p, q);
}

static tk_t lx_next__lparen(char * p, char * q) {
  return tk_make(TK_LPAREN, p, q);
}

static tk_t lx_next__rparen(char * p, char * q) {
  return tk_make(TK_RPAREN, p, q);
}

static tk_t lx_next__plus(char * p, char * q) {
  return tk_make(TK_ADD, p, q);
}

static tk_t lx_next__dash(char * p, char * q) {
  char c = * q;

  if (lx_is_digit(c)) {
    return lx_next__number(p, q + 1);
  }

  return tk_make(TK_SUB, p, q);
}

static tk_t lx_next__star(char * p, char * q) {
  return tk_make(TK_MUL, p, q);
}

static tk_t lx_next__slash(char * p, char * q) {
  return tk_make(TK_DIV, p, q);
}

static tk_t lx_next__tilde(char * p, char * q) {
  return tk_make(TK_NEG, p, q);
}

static tk_t lx_next__equal(char * p, char * q) {
  if (* q == '=') return tk_make(TK_EQ, p, q + 1);

  return tk_make(TK_ASSIGN, p, q);
}

static tk_t lx_next__bang(char * p, char * q) {
  if (* q == '=') return tk_make(TK_NE, p, q + 1);

  return tk_make(TK_ERROR, p, q);
}

static tk_t lx_next__langle(char * p, char * q) {
  if (* q == '=') return tk_make(TK_LE, p, q + 1);

  return tk_make(TK_LT, p, q);
}

static tk_t lx_next__rangle(char * p, char * q) {
  if (* q == '=') return tk_make(TK_GE, p, q + 1);

  return tk_make(TK_GT, p, q);
}

static tk_t (* lx_next__jump[])(char *, char *) = {
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
  lx_next__number, // 0
  lx_next__number,
  lx_next__number,
  lx_next__number,
  lx_next__number,
  lx_next__number,
  lx_next__number,
  lx_next__number,
  lx_next__number,
  lx_next__number, // 9
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
  lx_next__id, // b
  lx_next__id_c, // c
  lx_next__id_d, // d
  lx_next__id_e, // e
  lx_next__id_f, // f
  lx_next__id, // g
  lx_next__id, // h
  lx_next__id_i, // i
  lx_next__id, // j
  lx_next__id, // k
  lx_next__id_l, // l
  lx_next__id, // m
  lx_next__id, // n
  lx_next__id, // o
  lx_next__id, // p
  lx_next__id, // q
  lx_next__id, // r
  lx_next__id, // s
  lx_next__id_t, // t
  lx_next__id, // u
  lx_next__id, // v
  lx_next__id_w, // w
  lx_next__id, // x
  lx_next__id, // y
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
