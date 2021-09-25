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

// character class

enum {
  LX_CC_ALPHA,
  LX_CC_DIGIT,
  LX_CC_HASH,
  LX_CC_ILLEGAL,
  LX_CC_MINUS,
  LX_CC_NEWLINE,
  LX_CC_NULL,
  LX_CC_OP,
  LX_CC_PUNCT,
  LX_CC_UNDER,
  LX_CC_WHITE,
  LX_CC_COUNT
};

typedef u8 lx_cc_t;

// lexer state

enum {
  LX_ST_BEGIN,
  LX_ST_COMMENT,
  LX_ST_ERROR,
  LX_ST_ID,
  LX_ST_MINUS,
  LX_ST_NUM,
  LX_ST_OP,
  LX_ST_PUNCT,
  LX_ST_Z_EOF,
  LX_ST_Z_ERROR,
  LX_ST_Z_ID,
  LX_ST_Z_NUM,
  LX_ST_Z_OP,
  LX_ST_Z_PUNCT,
  LX_ST_COUNT,
};

#define LX_ST_NONTERMINAL_COUNT 8

typedef u8 lx_st_t;

static lx_cc_t lx_cc_table[];

static bool lx_ln_table[LX_ST_NONTERMINAL_COUNT];

static lx_st_t lx_tr_table[LX_ST_NONTERMINAL_COUNT][LX_CC_COUNT];

static tk_t (* lx_fn_table[LX_ST_COUNT])(char *, lx_st_t, i64);

static tk_t lx_next__loop(char * p, lx_st_t s, i64 n) {
  n = n + lx_ln_table[s];
  s = lx_tr_table[s][lx_cc_table[(u8) * p]];
  p = p + 1;

  return lx_fn_table[s](p, s, n);
}

static inline tk_t lx_next(lx_t * t) {
  char * p = t->next;
  tk_t r = lx_next__loop(p, LX_ST_BEGIN, 0);
  t->next = r.stop;
  return r;
}

static tk_t lx_next__eof(char * p, __attribute__((unused)) lx_st_t s, __attribute__((unused)) i64 n) {
  return tk_make(TK_EOF, p - 1, p - 1);
}

static tk_t lx_next__error(char * p, __attribute__((unused)) lx_st_t s, i64 n) {
  return tk_make(TK_ERROR, p - 1 - n, p - 1);
}

static tk_t lx_next__id(char * p, __attribute__((unused)) lx_st_t s, i64 n) {
  return tk_make(TK_ID, p - 1 - n, p - 1);
}

static tk_t lx_next__num(char * p, __attribute__((unused)) lx_st_t s, i64 n) {
  return tk_make(TK_NUM, p - 1 - n, p - 1);
}

static tk_t lx_next__op(char * p, __attribute__((unused)) lx_st_t s, i64 n) {
  return tk_make(TK_ADD, p - 1 - n, p - 1);
}

static tk_t lx_next__punct(char * p, __attribute__((unused)) lx_st_t s, __attribute__((unused)) i64 n) {
  return tk_make(TK_LPAREN, p - 2, p - 1);
}

static lx_cc_t lx_cc_table[] = {
  LX_CC_NULL, // \0
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_WHITE, // \t
  LX_CC_NEWLINE, // \n
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_WHITE, // ' '
  LX_CC_OP, // !
  LX_CC_ILLEGAL, // "
  LX_CC_HASH, // #
  LX_CC_OP, // $
  LX_CC_OP, // %
  LX_CC_OP, // &
  LX_CC_ILLEGAL, // '
  LX_CC_PUNCT, // (
  LX_CC_PUNCT, // )
  LX_CC_OP, // *
  LX_CC_OP, // +
  LX_CC_PUNCT, // ,
  LX_CC_MINUS, // -
  LX_CC_PUNCT, // .
  LX_CC_OP, // /
  LX_CC_DIGIT, // 0
  LX_CC_DIGIT,
  LX_CC_DIGIT,
  LX_CC_DIGIT,
  LX_CC_DIGIT,
  LX_CC_DIGIT,
  LX_CC_DIGIT,
  LX_CC_DIGIT,
  LX_CC_DIGIT,
  LX_CC_DIGIT, // 9
  LX_CC_PUNCT, // :
  LX_CC_PUNCT, // ;
  LX_CC_PUNCT, // <
  LX_CC_OP, // =
  LX_CC_PUNCT, // >
  LX_CC_OP, // ?
  LX_CC_OP, // @
  LX_CC_ALPHA, // A
  LX_CC_ALPHA,
  LX_CC_ALPHA,
  LX_CC_ALPHA,
  LX_CC_ALPHA,
  LX_CC_ALPHA,
  LX_CC_ALPHA,
  LX_CC_ALPHA,
  LX_CC_ALPHA,
  LX_CC_ALPHA,
  LX_CC_ALPHA,
  LX_CC_ALPHA,
  LX_CC_ALPHA,
  LX_CC_ALPHA,
  LX_CC_ALPHA,
  LX_CC_ALPHA,
  LX_CC_ALPHA,
  LX_CC_ALPHA,
  LX_CC_ALPHA,
  LX_CC_ALPHA,
  LX_CC_ALPHA,
  LX_CC_ALPHA,
  LX_CC_ALPHA,
  LX_CC_ALPHA,
  LX_CC_ALPHA,
  LX_CC_ALPHA, // Z
  LX_CC_PUNCT, // [
  LX_CC_ILLEGAL, // '\'
  LX_CC_PUNCT, // ]
  LX_CC_OP, // ^
  LX_CC_UNDER, // _
  LX_CC_ILLEGAL, // `
  LX_CC_ALPHA, // a
  LX_CC_ALPHA, // b
  LX_CC_ALPHA, // c
  LX_CC_ALPHA, // d
  LX_CC_ALPHA, // e
  LX_CC_ALPHA, // f
  LX_CC_ALPHA, // g
  LX_CC_ALPHA, // h
  LX_CC_ALPHA, // i
  LX_CC_ALPHA, // j
  LX_CC_ALPHA, // k
  LX_CC_ALPHA, // l
  LX_CC_ALPHA, // m
  LX_CC_ALPHA, // n
  LX_CC_ALPHA, // o
  LX_CC_ALPHA, // p
  LX_CC_ALPHA, // q
  LX_CC_ALPHA, // r
  LX_CC_ALPHA, // s
  LX_CC_ALPHA, // t
  LX_CC_ALPHA, // u
  LX_CC_ALPHA, // v
  LX_CC_ALPHA, // w
  LX_CC_ALPHA, // x
  LX_CC_ALPHA, // y
  LX_CC_ALPHA, // z
  LX_CC_PUNCT, // {
  LX_CC_OP, // |
  LX_CC_PUNCT, // }
  LX_CC_OP, // ~
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
  LX_CC_ILLEGAL,
};

static lx_st_t lx_tr_table[LX_ST_NONTERMINAL_COUNT][LX_CC_COUNT] = {
  // LX_ST_BEGIN
  {
    LX_ST_ID, // ALPHA
    LX_ST_NUM, // DIGIT
    LX_ST_COMMENT, // HASH
    LX_ST_ERROR, // ILLEGAL
    LX_ST_MINUS, // MINUS
    LX_ST_BEGIN, // NEWLINE
    LX_ST_Z_EOF, // NULL
    LX_ST_OP, // OP
    LX_ST_PUNCT, // PUNCT
    LX_ST_ERROR, // UNDER
    LX_ST_BEGIN, // WHITE
  },

  // LX_ST_COMMENT
  {
    LX_ST_COMMENT, // ALPHA
    LX_ST_COMMENT, // DIGIT
    LX_ST_COMMENT, // HASH
    LX_ST_COMMENT, // ILLEGAL
    LX_ST_COMMENT, // MINUS
    LX_ST_BEGIN, // NEWLINE
    LX_ST_Z_EOF, // NULL
    LX_ST_COMMENT, // OP
    LX_ST_COMMENT, // PUNCT
    LX_ST_COMMENT, // UNDER
    LX_ST_COMMENT, // WHITE
  },

  // LX_ST_ERROR
  {
    LX_ST_Z_ERROR, // ALPHA
    LX_ST_Z_ERROR, // DIGIT
    LX_ST_Z_ERROR, // HASH
    LX_ST_Z_ERROR, // ILLEGAL
    LX_ST_Z_ERROR, // MINUS
    LX_ST_Z_ERROR, // NEWLINE
    LX_ST_Z_ERROR, // NULL
    LX_ST_Z_ERROR, // OP
    LX_ST_Z_ERROR, // PUNCT
    LX_ST_Z_ERROR, // UNDER
    LX_ST_Z_ERROR, // WHITE
  },

  // LX_ST_ID
  {
    LX_ST_ID, // ALPHA
    LX_ST_ID, // DIGIT
    LX_ST_Z_ID, // HASH
    LX_ST_Z_ID, // ILLEGAL
    LX_ST_Z_ID, // MINUS
    LX_ST_Z_ID, // NEWLINE
    LX_ST_Z_ID, // NULL
    LX_ST_Z_ID, // OP
    LX_ST_Z_ID, // PUNCT
    LX_ST_ID, // UNDER
    LX_ST_Z_ID, // WHITE
  },

  // LX_ST_MINUS
  {
    LX_ST_Z_OP, // ALPHA
    LX_ST_NUM, // DIGIT
    LX_ST_Z_OP, // HASH
    LX_ST_Z_OP, // ILLEGAL
    LX_ST_OP, // MINUS
    LX_ST_Z_OP, // NEWLINE
    LX_ST_Z_OP, // NULL
    LX_ST_OP, // OP
    LX_ST_Z_OP, // PUNCT
    LX_ST_Z_OP, // UNDER
    LX_ST_Z_OP, // WHITE
  },

  // LX_ST_NUM
  {
    LX_ST_Z_NUM, // ALPHA
    LX_ST_NUM, // DIGIT
    LX_ST_Z_NUM, // HASH
    LX_ST_Z_NUM, // ILLEGAL
    LX_ST_Z_NUM, // MINUS
    LX_ST_Z_NUM, // NEWLINE
    LX_ST_Z_NUM, // NULL
    LX_ST_Z_NUM, // OP
    LX_ST_Z_NUM, // PUNCT
    LX_ST_Z_NUM, // UNDER
    LX_ST_Z_NUM, // WHITE
  },

  // LX_ST_OP
  {
    LX_ST_Z_OP, // ALPHA
    LX_ST_Z_OP, // DIGIT
    LX_ST_Z_OP, // HASH
    LX_ST_Z_OP, // ILLEGAL
    LX_ST_OP, // MINUS
    LX_ST_Z_OP, // NEWLINE
    LX_ST_Z_OP, // NULL
    LX_ST_OP, // OP
    LX_ST_Z_OP, // PUNCT
    LX_ST_Z_OP, // UNDER
    LX_ST_Z_OP, // WHITE
  },

  // LX_ST_PUNCT
  {
    LX_ST_Z_PUNCT, // ALPHA
    LX_ST_Z_PUNCT, // DIGIT
    LX_ST_Z_PUNCT, // HASH
    LX_ST_Z_PUNCT, // ILLEGAL
    LX_ST_Z_PUNCT, // MINUS
    LX_ST_Z_PUNCT, // NEWLINE
    LX_ST_Z_PUNCT, // NULL
    LX_ST_Z_PUNCT, // OP
    LX_ST_Z_PUNCT, // PUNCT
    LX_ST_Z_PUNCT, // UNDER
    LX_ST_Z_PUNCT, // WHITE
  },

};

static bool lx_ln_table[LX_ST_NONTERMINAL_COUNT] = {
  false,
  false,
  true,
  true,
  true,
  true,
  true,
  true,
};

static tk_t (* lx_fn_table[LX_ST_COUNT])(char *, lx_st_t, i64) = {
  lx_next__loop, // BEGIN
  lx_next__loop, // COMMENT
  lx_next__loop, // ERROR
  lx_next__loop, // ID
  lx_next__loop, // MINUS
  lx_next__loop, // NUM
  lx_next__loop, // OP
  lx_next__loop, // PUNCT
  lx_next__eof, // Z_EOF
  lx_next__error, // Z_ERROR
  lx_next__id, // Z_ID
  lx_next__num, // Z_NUM
  lx_next__op, // Z_OP
  lx_next__punct, // Z_PUNCT
};
