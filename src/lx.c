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
  return (lx_t) { .next = source };
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
  LX_ST_AA_COMMENT,
  LX_ST_AA_START,
  LX_ST_ID,
  LX_ST_MINUS,
  LX_ST_NUM,
  LX_ST_OP,
  LX_ST_ZZ_EOF,
  LX_ST_ZZ_ERROR,
  LX_ST_ZZ_ID,
  LX_ST_ZZ_NUM,
  LX_ST_ZZ_OP,
  LX_ST_ZZ_PUNCT,
  LX_ST_COUNT,
};

#define LX_ST_SKIP_COUNT 2

typedef u8 lx_st_t;

static lx_cc_t lx_cc_table[];

static lx_st_t lx_tr_table[][LX_CC_COUNT];

static tk_t (* lx_fn_table[LX_ST_COUNT])(char *, lx_st_t, i64);

static tk_t lx_next__loop(char * p, lx_st_t s, i64 n) {
  n = n + (s >= LX_ST_SKIP_COUNT);
  s = lx_tr_table[s][lx_cc_table[(u8) * p]];
  p = p + 1;

  return lx_fn_table[s](p, s, n);
}

static inline tk_t lx_next(lx_t * t) {
  char * p = t->next;
  tk_t r = lx_next__loop(p, LX_ST_AA_START, 0);
  t->next = r.stop;
  return r;
}

static tk_t lx_next__eof(char * p, __attribute__((unused)) lx_st_t s, __attribute__((unused)) i64 n) {
  char * start = p - 1;
  char * stop = p - 1;

  return tk_make(TK_EOF, start, stop);
}

static tk_t lx_next__error(char * p, __attribute__((unused)) lx_st_t s, i64 n) {
  char * start = p - 1 - n;
  char * stop = p;

  return tk_make(TK_ERROR, start, stop);
}

static tk_t lx_next__id(char * p, __attribute__((unused)) lx_st_t s, i64 n) {
  char * start = p - 1 - n;
  char * stop = p - 1;

  switch (n) {
    case 2:
      if (!bcmp(start, "do", 2)) return tk_make(TK_DO, start, stop);
      if (!bcmp(start, "if", 2)) return tk_make(TK_IF, start, stop);
      break;
    case 3:
      if (!bcmp(start, "end", 3)) return tk_make(TK_END, start, stop);
      if (!bcmp(start, "for", 3)) return tk_make(TK_FOR, start, stop);
      if (!bcmp(start, "fun", 3)) return tk_make(TK_FUN, start, stop);
      if (!bcmp(start, "let", 3)) return tk_make(TK_LET, start, stop);
      break;
    case 4:
      if (!bcmp(start, "case", 4)) return tk_make(TK_CASE, start, stop);
      if (!bcmp(start, "elif", 4)) return tk_make(TK_ELIF, start, stop);
      if (!bcmp(start, "else", 4)) return tk_make(TK_ELSE, start, stop);
      if (!bcmp(start, "then", 4)) return tk_make(TK_THEN, start, stop);
      break;
    case 5:
      if (!bcmp(start, "while", 5)) return tk_make(TK_WHILE, start, stop);
      break;
  }

  return tk_make(TK_ID, start, stop);
}

static tk_t lx_next__num(char * p, __attribute__((unused)) lx_st_t s, i64 n) {
  char * start = p - 1 - n;
  char * stop = p - 1;

  return tk_make(TK_NUM, start, stop);
}

static tk_t lx_next__op(char * p, __attribute__((unused)) lx_st_t s, i64 n) {
  char * start = p - 1 - n;
  char * stop = p - 1;

  switch (n) {
    case 1:
      switch (* start) {
        case '=': return tk_make(TK_ASSIGN, start, stop);
        case '+': return tk_make(TK_ADD, start, stop);
        case '-': return tk_make(TK_SUB, start, stop);
        case '*': return tk_make(TK_MUL, start, stop);
        case '~': return tk_make(TK_NEG, start, stop);
        case '/': return tk_make(TK_DIV, start, stop);
        case '<': return tk_make(TK_LT, start, stop);
        case '>': return tk_make(TK_GT, start, stop);
      }
      break;
    case 2:
      if (* (start + 1) == '=') {
        switch (* start) {
          case '=': return tk_make(TK_EQ, start, stop);
          case '!': return tk_make(TK_NE, start, stop);
          case '<': return tk_make(TK_LE, start, stop);
          case '>': return tk_make(TK_GE, start, stop);
        }
      }
      break;
  };

  return tk_make(TK_ERROR, start, stop);
}

static tk_t lx_next__punct(char * p, __attribute__((unused)) lx_st_t s, __attribute__((unused)) i64 n) {
  char * start = p - 1;
  char * stop = p;

  switch (* start) {
    case ',': return tk_make(TK_COMMA, start, stop);
    case '.': return tk_make(TK_DOT, start, stop);
    case ':': return tk_make(TK_COLON, start, stop);
    case ';': return tk_make(TK_SEMICOLON, start, stop);
    case '{': return tk_make(TK_LBRACE, start, stop);
    case '}': return tk_make(TK_RBRACE, start, stop);
    case '[': return tk_make(TK_LBRACKET, start, stop);
    case ']': return tk_make(TK_RBRACKET, start, stop);
    case '(': return tk_make(TK_LPAREN, start, stop);
    case ')': return tk_make(TK_RPAREN, start, stop);
  };

  return tk_make(TK_ERROR, start, stop);
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
  LX_CC_OP, // <
  LX_CC_OP, // =
  LX_CC_OP, // >
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

static lx_st_t lx_tr_table[][LX_CC_COUNT] = {
  // LX_ST_AA_COMMENT
  {
    LX_ST_AA_COMMENT, // ALPHA
    LX_ST_AA_COMMENT, // DIGIT
    LX_ST_AA_COMMENT, // HASH
    LX_ST_AA_COMMENT, // ILLEGAL
    LX_ST_AA_COMMENT, // MINUS
    LX_ST_AA_START, // NEWLINE
    LX_ST_ZZ_EOF, // NULL
    LX_ST_AA_COMMENT, // OP
    LX_ST_AA_COMMENT, // PUNCT
    LX_ST_AA_COMMENT, // UNDER
    LX_ST_AA_COMMENT, // WHITE
  },

  // LX_ST_AA_START
  {
    LX_ST_ID, // ALPHA
    LX_ST_NUM, // DIGIT
    LX_ST_AA_COMMENT, // HASH
    LX_ST_ZZ_ERROR, // ILLEGAL
    LX_ST_MINUS, // MINUS
    LX_ST_AA_START, // NEWLINE
    LX_ST_ZZ_EOF, // NULL
    LX_ST_OP, // OP
    LX_ST_ZZ_PUNCT, // PUNCT
    LX_ST_ZZ_ERROR, // UNDER
    LX_ST_AA_START, // WHITE
  },

  // LX_ST_ID
  {
    LX_ST_ID, // ALPHA
    LX_ST_ID, // DIGIT
    LX_ST_ZZ_ID, // HASH
    LX_ST_ZZ_ID, // ILLEGAL
    LX_ST_ZZ_ID, // MINUS
    LX_ST_ZZ_ID, // NEWLINE
    LX_ST_ZZ_ID, // NULL
    LX_ST_ZZ_ID, // OP
    LX_ST_ZZ_ID, // PUNCT
    LX_ST_ID, // UNDER
    LX_ST_ZZ_ID, // WHITE
  },

  // LX_ST_MINUS
  {
    LX_ST_ZZ_OP, // ALPHA
    LX_ST_NUM, // DIGIT
    LX_ST_ZZ_OP, // HASH
    LX_ST_ZZ_OP, // ILLEGAL
    LX_ST_OP, // MINUS
    LX_ST_ZZ_OP, // NEWLINE
    LX_ST_ZZ_OP, // NULL
    LX_ST_OP, // OP
    LX_ST_ZZ_OP, // PUNCT
    LX_ST_ZZ_OP, // UNDER
    LX_ST_ZZ_OP, // WHITE
  },

  // LX_ST_NUM
  {
    LX_ST_ZZ_NUM, // ALPHA
    LX_ST_NUM, // DIGIT
    LX_ST_ZZ_NUM, // HASH
    LX_ST_ZZ_NUM, // ILLEGAL
    LX_ST_ZZ_NUM, // MINUS
    LX_ST_ZZ_NUM, // NEWLINE
    LX_ST_ZZ_NUM, // NULL
    LX_ST_ZZ_NUM, // OP
    LX_ST_ZZ_NUM, // PUNCT
    LX_ST_ZZ_NUM, // UNDER
    LX_ST_ZZ_NUM, // WHITE
  },

  // LX_ST_OP
  {
    LX_ST_ZZ_OP, // ALPHA
    LX_ST_ZZ_OP, // DIGIT
    LX_ST_ZZ_OP, // HASH
    LX_ST_ZZ_OP, // ILLEGAL
    LX_ST_OP, // MINUS
    LX_ST_ZZ_OP, // NEWLINE
    LX_ST_ZZ_OP, // NULL
    LX_ST_OP, // OP
    LX_ST_ZZ_OP, // PUNCT
    LX_ST_ZZ_OP, // UNDER
    LX_ST_ZZ_OP, // WHITE
  },

};

static tk_t (* lx_fn_table[LX_ST_COUNT])(char *, lx_st_t, i64) = {
  lx_next__loop, // AA_COMMENT
  lx_next__loop, // AA_START
  lx_next__loop, // ID
  lx_next__loop, // MINUS
  lx_next__loop, // NUM
  lx_next__loop, // OP
  lx_next__eof, // ZZ_EOF
  lx_next__error, // ZZ_ERROR
  lx_next__id, // ZZ_ID
  lx_next__num, // ZZ_NUM
  lx_next__op, // ZZ_OP
  lx_next__punct, // ZZ_PUNCT
};
