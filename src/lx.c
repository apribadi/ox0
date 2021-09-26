// lexing
//
// lx_t
//
// lx_t lx_make(char * source)
//
// tk_t lx_step(lx_t * t)

typedef struct {
  char * pos;
} lx_t;

// Source MUST be terminated by a '\0' character.

static inline lx_t lx_make(char * source) {
  return (lx_t) { .pos = source };
}

// character class

enum {
  LX_CLASS_ALPHA,
  LX_CLASS_DIGIT,
  LX_CLASS_HASH,
  LX_CLASS_ILLEGAL,
  LX_CLASS_MINUS,
  LX_CLASS_NEWLINE,
  LX_CLASS_NULL,
  LX_CLASS_OP,
  LX_CLASS_PUNCT,
  LX_CLASS_WHITESPACE,
  LX_CLASS_COUNT
};

typedef u8 lx_class_t;

// lexer state

enum {
  LX_STATE_ID,
  LX_STATE_MINUS,
  LX_STATE_NUMBER,
  LX_STATE_OP,
  LX_STATE_YY_COMMENT,
  LX_STATE_YY_START,
  LX_STATE_ZZ_EOF,
  LX_STATE_ZZ_ERROR,
  LX_STATE_ZZ_ID,
  LX_STATE_ZZ_NUMBER,
  LX_STATE_ZZ_OP,
  LX_STATE_ZZ_PUNCT,
  LX_STATE_COUNT,
};

#define LX_STATE_INCLUDED_COUNT 4

typedef u8 lx_state_t;

static lx_class_t lx_class_table[];

static lx_state_t lx_transition_table[][LX_CLASS_COUNT];

static tk_t (* lx_jump_table[])(char *, lx_state_t, i64);

static tk_t lx_step__loop(char * p, lx_state_t s, i64 n) {
  n = n + (s < LX_STATE_INCLUDED_COUNT);
  s = lx_transition_table[s][lx_class_table[(u8) * p]];
  p = p + 1;

  return lx_jump_table[s](p, s, n);
}

static inline tk_t lx_step(lx_t * t) {
  char * p = t->pos;
  tk_t r = lx_step__loop(p, LX_STATE_YY_START, 0);
  t->pos = r.stop;
  return r;
}

static tk_t lx_step__eof(char * p, __attribute__((unused)) lx_state_t s, __attribute__((unused)) i64 n) {
  char * start = p - 1;
  char * stop = p - 1;

  return tk_make(TK_EOF, start, stop);
}

static tk_t lx_step__error(char * p, __attribute__((unused)) lx_state_t s, i64 n) {
  char * start = p - 1 - n;
  char * stop = p;

  return tk_make(TK_ERROR, start, stop);
}

static tk_t lx_step__id(char * p, __attribute__((unused)) lx_state_t s, i64 n) {
  char * start = p - 1 - n;
  char * stop = p - 1;

  switch (n) {
    case 2:
      if (!bcmp(start, "do", 2)) return tk_make(TK_DO, start, stop);
      if (!bcmp(start, "if", 2)) return tk_make(TK_IF, start, stop);
      if (!bcmp(start, "or", 2)) return tk_make(TK_OR, start, stop);
      break;
    case 3:
      if (!bcmp(start, "and", 3)) return tk_make(TK_AND, start, stop);
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

static tk_t lx_step__number(char * p, __attribute__((unused)) lx_state_t s, i64 n) {
  char * start = p - 1 - n;
  char * stop = p - 1;

  return tk_make(TK_NUMBER, start, stop);
}

static tk_t lx_step__op(char * p, __attribute__((unused)) lx_state_t s, i64 n) {
  char * start = p - 1 - n;
  char * stop = p - 1;

  switch (n) {
    case 1:
      switch (* start) {
        case '=': return tk_make(TK_ASSIGNMENT, start, stop);
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

static tk_t lx_step__punct(char * p, __attribute__((unused)) lx_state_t s, __attribute__((unused)) i64 n) {
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

static lx_class_t lx_class_table[] = {
  LX_CLASS_NULL, // \0
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_WHITESPACE, // \t
  LX_CLASS_NEWLINE, // \n
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_WHITESPACE, // ' '
  LX_CLASS_OP, // !
  LX_CLASS_ILLEGAL, // "
  LX_CLASS_HASH, // #
  LX_CLASS_OP, // $
  LX_CLASS_OP, // %
  LX_CLASS_OP, // &
  LX_CLASS_ILLEGAL, // '
  LX_CLASS_PUNCT, // (
  LX_CLASS_PUNCT, // )
  LX_CLASS_OP, // *
  LX_CLASS_OP, // +
  LX_CLASS_PUNCT, // ,
  LX_CLASS_MINUS, // -
  LX_CLASS_PUNCT, // .
  LX_CLASS_OP, // /
  LX_CLASS_DIGIT, // 0
  LX_CLASS_DIGIT,
  LX_CLASS_DIGIT,
  LX_CLASS_DIGIT,
  LX_CLASS_DIGIT,
  LX_CLASS_DIGIT,
  LX_CLASS_DIGIT,
  LX_CLASS_DIGIT,
  LX_CLASS_DIGIT,
  LX_CLASS_DIGIT, // 9
  LX_CLASS_PUNCT, // :
  LX_CLASS_PUNCT, // ;
  LX_CLASS_OP, // <
  LX_CLASS_OP, // =
  LX_CLASS_OP, // >
  LX_CLASS_OP, // ?
  LX_CLASS_OP, // @
  LX_CLASS_ALPHA, // A
  LX_CLASS_ALPHA,
  LX_CLASS_ALPHA,
  LX_CLASS_ALPHA,
  LX_CLASS_ALPHA,
  LX_CLASS_ALPHA,
  LX_CLASS_ALPHA,
  LX_CLASS_ALPHA,
  LX_CLASS_ALPHA,
  LX_CLASS_ALPHA,
  LX_CLASS_ALPHA,
  LX_CLASS_ALPHA,
  LX_CLASS_ALPHA,
  LX_CLASS_ALPHA,
  LX_CLASS_ALPHA,
  LX_CLASS_ALPHA,
  LX_CLASS_ALPHA,
  LX_CLASS_ALPHA,
  LX_CLASS_ALPHA,
  LX_CLASS_ALPHA,
  LX_CLASS_ALPHA,
  LX_CLASS_ALPHA,
  LX_CLASS_ALPHA,
  LX_CLASS_ALPHA,
  LX_CLASS_ALPHA,
  LX_CLASS_ALPHA, // Z
  LX_CLASS_PUNCT, // [
  LX_CLASS_ILLEGAL, // '\'
  LX_CLASS_PUNCT, // ]
  LX_CLASS_OP, // ^
  LX_CLASS_ALPHA, // _
  LX_CLASS_ILLEGAL, // `
  LX_CLASS_ALPHA, // a
  LX_CLASS_ALPHA, // b
  LX_CLASS_ALPHA, // c
  LX_CLASS_ALPHA, // d
  LX_CLASS_ALPHA, // e
  LX_CLASS_ALPHA, // f
  LX_CLASS_ALPHA, // g
  LX_CLASS_ALPHA, // h
  LX_CLASS_ALPHA, // i
  LX_CLASS_ALPHA, // j
  LX_CLASS_ALPHA, // k
  LX_CLASS_ALPHA, // l
  LX_CLASS_ALPHA, // m
  LX_CLASS_ALPHA, // n
  LX_CLASS_ALPHA, // o
  LX_CLASS_ALPHA, // p
  LX_CLASS_ALPHA, // q
  LX_CLASS_ALPHA, // r
  LX_CLASS_ALPHA, // s
  LX_CLASS_ALPHA, // t
  LX_CLASS_ALPHA, // u
  LX_CLASS_ALPHA, // v
  LX_CLASS_ALPHA, // w
  LX_CLASS_ALPHA, // x
  LX_CLASS_ALPHA, // y
  LX_CLASS_ALPHA, // z
  LX_CLASS_PUNCT, // {
  LX_CLASS_OP, // |
  LX_CLASS_PUNCT, // }
  LX_CLASS_OP, // ~
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
  LX_CLASS_ILLEGAL,
};

static lx_state_t lx_transition_table[][LX_CLASS_COUNT] = {
  [LX_STATE_ID] = {
    LX_STATE_ID, // ALPHA
    LX_STATE_ID, // DIGIT
    LX_STATE_ZZ_ID, // HASH
    LX_STATE_ZZ_ID, // ILLEGAL
    LX_STATE_ZZ_ID, // MINUS
    LX_STATE_ZZ_ID, // NEWLINE
    LX_STATE_ZZ_ID, // NULL
    LX_STATE_ZZ_ID, // OP
    LX_STATE_ZZ_ID, // PUNCT
    LX_STATE_ZZ_ID, // WHITESPACE
  },
  [LX_STATE_MINUS] = {
    LX_STATE_ZZ_OP, // ALPHA
    LX_STATE_NUMBER, // DIGIT
    LX_STATE_ZZ_OP, // HASH
    LX_STATE_ZZ_OP, // ILLEGAL
    LX_STATE_OP, // MINUS
    LX_STATE_ZZ_OP, // NEWLINE
    LX_STATE_ZZ_OP, // NULL
    LX_STATE_OP, // OP
    LX_STATE_ZZ_OP, // PUNCT
    LX_STATE_ZZ_OP, // WHITESPACE
  },
  [LX_STATE_NUMBER] = {
    LX_STATE_ZZ_NUMBER, // ALPHA
    LX_STATE_NUMBER, // DIGIT
    LX_STATE_ZZ_NUMBER, // HASH
    LX_STATE_ZZ_NUMBER, // ILLEGAL
    LX_STATE_ZZ_NUMBER, // MINUS
    LX_STATE_ZZ_NUMBER, // NEWLINE
    LX_STATE_ZZ_NUMBER, // NULL
    LX_STATE_ZZ_NUMBER, // OP
    LX_STATE_ZZ_NUMBER, // PUNCT
    LX_STATE_ZZ_NUMBER, // WHITESPACE
  },
  [LX_STATE_OP] = {
    LX_STATE_ZZ_OP, // ALPHA
    LX_STATE_ZZ_OP, // DIGIT
    LX_STATE_ZZ_OP, // HASH
    LX_STATE_ZZ_OP, // ILLEGAL
    LX_STATE_OP, // MINUS
    LX_STATE_ZZ_OP, // NEWLINE
    LX_STATE_ZZ_OP, // NULL
    LX_STATE_OP, // OP
    LX_STATE_ZZ_OP, // PUNCT
    LX_STATE_ZZ_OP, // WHITESPACE
  },
  [LX_STATE_YY_COMMENT] = {
    LX_STATE_YY_COMMENT, // ALPHA
    LX_STATE_YY_COMMENT, // DIGIT
    LX_STATE_YY_COMMENT, // HASH
    LX_STATE_YY_COMMENT, // ILLEGAL
    LX_STATE_YY_COMMENT, // MINUS
    LX_STATE_YY_START, // NEWLINE
    LX_STATE_ZZ_EOF, // NULL
    LX_STATE_YY_COMMENT, // OP
    LX_STATE_YY_COMMENT, // PUNCT
    LX_STATE_YY_COMMENT, // WHITESPACE
  },
  [LX_STATE_YY_START] = {
    LX_STATE_ID, // ALPHA
    LX_STATE_NUMBER, // DIGIT
    LX_STATE_YY_COMMENT, // HASH
    LX_STATE_ZZ_ERROR, // ILLEGAL
    LX_STATE_MINUS, // MINUS
    LX_STATE_YY_START, // NEWLINE
    LX_STATE_ZZ_EOF, // NULL
    LX_STATE_OP, // OP
    LX_STATE_ZZ_PUNCT, // PUNCT
    LX_STATE_YY_START, // WHITESPACE
  },
};

static tk_t (* lx_jump_table[LX_STATE_COUNT])(char *, lx_state_t, i64) = {
  [LX_STATE_ID] = lx_step__loop,
  [LX_STATE_MINUS] = lx_step__loop,
  [LX_STATE_NUMBER] = lx_step__loop,
  [LX_STATE_OP] = lx_step__loop,
  [LX_STATE_YY_COMMENT] = lx_step__loop,
  [LX_STATE_YY_START] = lx_step__loop,
  [LX_STATE_ZZ_EOF] = lx_step__eof,
  [LX_STATE_ZZ_ERROR] = lx_step__error,
  [LX_STATE_ZZ_ID] = lx_step__id,
  [LX_STATE_ZZ_NUMBER] = lx_step__number,
  [LX_STATE_ZZ_OP] = lx_step__op,
  [LX_STATE_ZZ_PUNCT] = lx_step__punct,
};
