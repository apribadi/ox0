// lexing
//
// lx_t
//
// lx_t lx_make(char * source)
//
// lx_token_t lx_step(lx_t * t)

enum {
  // punctuation

  LX_TOKEN_ASSIGNMENT,
  LX_TOKEN_COMMA,
  LX_TOKEN_DOT,
  LX_TOKEN_COLON,
  LX_TOKEN_SEMICOLON,
  LX_TOKEN_LBRACE,
  LX_TOKEN_RBRACE,
  LX_TOKEN_LBRACKET,
  LX_TOKEN_RBRACKET,
  LX_TOKEN_LPAREN,
  LX_TOKEN_RPAREN,

  // operator

  LX_TOKEN_ADD,
  LX_TOKEN_SUB,
  LX_TOKEN_MUL,
  LX_TOKEN_DIV,
  LX_TOKEN_NEG,
  LX_TOKEN_EQ,
  LX_TOKEN_NE,
  LX_TOKEN_GE,
  LX_TOKEN_GT,
  LX_TOKEN_LE,
  LX_TOKEN_LT,

  // keyword

  LX_TOKEN_AND,
  LX_TOKEN_CASE,
  LX_TOKEN_DO,
  LX_TOKEN_ELIF,
  LX_TOKEN_ELSE,
  LX_TOKEN_END,
  LX_TOKEN_FOR,
  LX_TOKEN_FUN,
  LX_TOKEN_IF,
  LX_TOKEN_LET,
  LX_TOKEN_OR,
  LX_TOKEN_THEN,
  LX_TOKEN_WHILE,

  LX_TOKEN_ID,
  LX_TOKEN_NUMBER,

  LX_TOKEN_EOF,
  LX_TOKEN_ERROR,
};

typedef u8 lx_token_tag_t;

typedef struct {
  lx_token_tag_t tag;
  char * start;
  char * stop;
} lx_token_t;

static inline lx_token_t lx_token_make(lx_token_tag_t tag, char * start, char * stop) {
  return (lx_token_t) { .tag = tag, .start = start, .stop = stop };
}

i64 lx_token_len(lx_token_t t) {
  return t.stop - t.start;
}

static char const * lx_token_name(lx_token_t t) {
  switch (t.tag) {
    case LX_TOKEN_ASSIGNMENT: return "ASSIGNMENT";
    case LX_TOKEN_COMMA: return "COMMA";
    case LX_TOKEN_DOT: return "DOT";
    case LX_TOKEN_COLON: return "COLON";
    case LX_TOKEN_SEMICOLON: return "SEMICOLON";
    case LX_TOKEN_LBRACE: return "LBRACE";
    case LX_TOKEN_RBRACE: return "RBRACE";
    case LX_TOKEN_LBRACKET: return "LBRACKET";
    case LX_TOKEN_RBRACKET: return "RBRACKET";
    case LX_TOKEN_LPAREN: return "LPAREN";
    case LX_TOKEN_RPAREN: return "RPAREN";
    case LX_TOKEN_ADD: return "ADD";
    case LX_TOKEN_SUB: return "SUB";
    case LX_TOKEN_MUL: return "MUL";
    case LX_TOKEN_DIV: return "DIV";
    case LX_TOKEN_NEG: return "NEG";
    case LX_TOKEN_EQ: return "EQ";
    case LX_TOKEN_NE: return "NE";
    case LX_TOKEN_GE: return "GE";
    case LX_TOKEN_GT: return "GT";
    case LX_TOKEN_LE: return "LE";
    case LX_TOKEN_LT: return "LT";
    case LX_TOKEN_AND: return "AND";
    case LX_TOKEN_CASE: return "CASE";
    case LX_TOKEN_DO: return "DO";
    case LX_TOKEN_ELIF: return "ELIF";
    case LX_TOKEN_ELSE: return "ELSE";
    case LX_TOKEN_END: return "END";
    case LX_TOKEN_FOR: return "FOR";
    case LX_TOKEN_FUN: return "FUN";
    case LX_TOKEN_IF: return "IF";
    case LX_TOKEN_LET: return "LET";
    case LX_TOKEN_OR: return "OR";
    case LX_TOKEN_THEN: return "THEN";
    case LX_TOKEN_WHILE: return "WHILE";
    case LX_TOKEN_ID: return "ID";
    case LX_TOKEN_NUMBER: return "NUMBER";
    case LX_TOKEN_EOF: return "EOF";
    case LX_TOKEN_ERROR: return "ERROR";
  }

  return "UNKNOWN";
}

static void lx_token_show(lx_token_t t) {
  printf("%s: %.*s\n", lx_token_name(t), (int) lx_token_len(t), t.start);
}

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
  LX_STATE_Y_COMMENT,
  LX_STATE_Y_START,
  LX_STATE_Z_EOF,
  LX_STATE_Z_ERROR,
  LX_STATE_Z_ID,
  LX_STATE_Z_NUMBER,
  LX_STATE_Z_OP,
  LX_STATE_Z_PUNCT,
  LX_STATE_COUNT,
};

#define LX_STATE_INCLUDED_COUNT 4

typedef u8 lx_state_t;

static lx_class_t lx_class_table[];

static lx_state_t lx_transition_table[][LX_CLASS_COUNT];

static lx_token_t (* lx_jump_table[])(char *, lx_state_t, i64);

static lx_token_t lx_step__loop(char * p, lx_state_t s, i64 n) {
  n = n + (s < LX_STATE_INCLUDED_COUNT);
  s = lx_transition_table[s][lx_class_table[(u8) * p]];
  p = p + 1;

  return lx_jump_table[s](p, s, n);
}

static inline lx_token_t lx_step(lx_t * t) {
  char * p = t->pos;
  lx_token_t r = lx_step__loop(p, LX_STATE_Y_START, 0);
  t->pos = r.stop;
  return r;
}

static lx_token_t lx_step__eof(char * p, __attribute__((unused)) lx_state_t s, __attribute__((unused)) i64 n) {
  char * start = p - 1;
  char * stop = p - 1;

  return lx_token_make(LX_TOKEN_EOF, start, stop);
}

static lx_token_t lx_step__error(char * p, __attribute__((unused)) lx_state_t s, i64 n) {
  char * start = p - 1 - n;
  char * stop = p;

  return lx_token_make(LX_TOKEN_ERROR, start, stop);
}

static lx_token_t lx_step__id(char * p, __attribute__((unused)) lx_state_t s, i64 n) {
  char * start = p - 1 - n;
  char * stop = p - 1;

  switch (n) {
    case 2:
      if (!bcmp(start, "do", 2)) return lx_token_make(LX_TOKEN_DO, start, stop);
      if (!bcmp(start, "if", 2)) return lx_token_make(LX_TOKEN_IF, start, stop);
      if (!bcmp(start, "or", 2)) return lx_token_make(LX_TOKEN_OR, start, stop);
      break;
    case 3:
      if (!bcmp(start, "and", 3)) return lx_token_make(LX_TOKEN_AND, start, stop);
      if (!bcmp(start, "end", 3)) return lx_token_make(LX_TOKEN_END, start, stop);
      if (!bcmp(start, "for", 3)) return lx_token_make(LX_TOKEN_FOR, start, stop);
      if (!bcmp(start, "fun", 3)) return lx_token_make(LX_TOKEN_FUN, start, stop);
      if (!bcmp(start, "let", 3)) return lx_token_make(LX_TOKEN_LET, start, stop);
      break;
    case 4:
      if (!bcmp(start, "case", 4)) return lx_token_make(LX_TOKEN_CASE, start, stop);
      if (!bcmp(start, "elif", 4)) return lx_token_make(LX_TOKEN_ELIF, start, stop);
      if (!bcmp(start, "else", 4)) return lx_token_make(LX_TOKEN_ELSE, start, stop);
      if (!bcmp(start, "then", 4)) return lx_token_make(LX_TOKEN_THEN, start, stop);
      break;
    case 5:
      if (!bcmp(start, "while", 5)) return lx_token_make(LX_TOKEN_WHILE, start, stop);
      break;
  }

  return lx_token_make(LX_TOKEN_ID, start, stop);
}

static lx_token_t lx_step__number(char * p, __attribute__((unused)) lx_state_t s, i64 n) {
  char * start = p - 1 - n;
  char * stop = p - 1;

  return lx_token_make(LX_TOKEN_NUMBER, start, stop);
}

static lx_token_t lx_step__op(char * p, __attribute__((unused)) lx_state_t s, i64 n) {
  char * start = p - 1 - n;
  char * stop = p - 1;

  switch (n) {
    case 1:
      switch (* start) {
        case '=': return lx_token_make(LX_TOKEN_ASSIGNMENT, start, stop);
        case '+': return lx_token_make(LX_TOKEN_ADD, start, stop);
        case '-': return lx_token_make(LX_TOKEN_SUB, start, stop);
        case '*': return lx_token_make(LX_TOKEN_MUL, start, stop);
        case '~': return lx_token_make(LX_TOKEN_NEG, start, stop);
        case '/': return lx_token_make(LX_TOKEN_DIV, start, stop);
        case '<': return lx_token_make(LX_TOKEN_LT, start, stop);
        case '>': return lx_token_make(LX_TOKEN_GT, start, stop);
      }
      break;
    case 2:
      if (* (start + 1) == '=') {
        switch (* start) {
          case '=': return lx_token_make(LX_TOKEN_EQ, start, stop);
          case '!': return lx_token_make(LX_TOKEN_NE, start, stop);
          case '<': return lx_token_make(LX_TOKEN_LE, start, stop);
          case '>': return lx_token_make(LX_TOKEN_GE, start, stop);
        }
      }
      break;
  };

  return lx_token_make(LX_TOKEN_ERROR, start, stop);
}

static lx_token_t lx_step__punct(char * p, __attribute__((unused)) lx_state_t s, __attribute__((unused)) i64 n) {
  char * start = p - 1;
  char * stop = p;

  switch (* start) {
    case ',': return lx_token_make(LX_TOKEN_COMMA, start, stop);
    case '.': return lx_token_make(LX_TOKEN_DOT, start, stop);
    case ':': return lx_token_make(LX_TOKEN_COLON, start, stop);
    case ';': return lx_token_make(LX_TOKEN_SEMICOLON, start, stop);
    case '{': return lx_token_make(LX_TOKEN_LBRACE, start, stop);
    case '}': return lx_token_make(LX_TOKEN_RBRACE, start, stop);
    case '[': return lx_token_make(LX_TOKEN_LBRACKET, start, stop);
    case ']': return lx_token_make(LX_TOKEN_RBRACKET, start, stop);
    case '(': return lx_token_make(LX_TOKEN_LPAREN, start, stop);
    case ')': return lx_token_make(LX_TOKEN_RPAREN, start, stop);
  };

  return lx_token_make(LX_TOKEN_ERROR, start, stop);
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
    LX_STATE_Z_ID, // HASH
    LX_STATE_Z_ID, // ILLEGAL
    LX_STATE_Z_ID, // MINUS
    LX_STATE_Z_ID, // NEWLINE
    LX_STATE_Z_ID, // NULL
    LX_STATE_Z_ID, // OP
    LX_STATE_Z_ID, // PUNCT
    LX_STATE_Z_ID, // WHITESPACE
  },
  [LX_STATE_MINUS] = {
    LX_STATE_Z_OP, // ALPHA
    LX_STATE_NUMBER, // DIGIT
    LX_STATE_Z_OP, // HASH
    LX_STATE_Z_OP, // ILLEGAL
    LX_STATE_OP, // MINUS
    LX_STATE_Z_OP, // NEWLINE
    LX_STATE_Z_OP, // NULL
    LX_STATE_OP, // OP
    LX_STATE_Z_OP, // PUNCT
    LX_STATE_Z_OP, // WHITESPACE
  },
  [LX_STATE_NUMBER] = {
    LX_STATE_Z_NUMBER, // ALPHA
    LX_STATE_NUMBER, // DIGIT
    LX_STATE_Z_NUMBER, // HASH
    LX_STATE_Z_NUMBER, // ILLEGAL
    LX_STATE_Z_NUMBER, // MINUS
    LX_STATE_Z_NUMBER, // NEWLINE
    LX_STATE_Z_NUMBER, // NULL
    LX_STATE_Z_NUMBER, // OP
    LX_STATE_Z_NUMBER, // PUNCT
    LX_STATE_Z_NUMBER, // WHITESPACE
  },
  [LX_STATE_OP] = {
    LX_STATE_Z_OP, // ALPHA
    LX_STATE_Z_OP, // DIGIT
    LX_STATE_Z_OP, // HASH
    LX_STATE_Z_OP, // ILLEGAL
    LX_STATE_OP, // MINUS
    LX_STATE_Z_OP, // NEWLINE
    LX_STATE_Z_OP, // NULL
    LX_STATE_OP, // OP
    LX_STATE_Z_OP, // PUNCT
    LX_STATE_Z_OP, // WHITESPACE
  },
  [LX_STATE_Y_COMMENT] = {
    LX_STATE_Y_COMMENT, // ALPHA
    LX_STATE_Y_COMMENT, // DIGIT
    LX_STATE_Y_COMMENT, // HASH
    LX_STATE_Y_COMMENT, // ILLEGAL
    LX_STATE_Y_COMMENT, // MINUS
    LX_STATE_Y_START, // NEWLINE
    LX_STATE_Z_EOF, // NULL
    LX_STATE_Y_COMMENT, // OP
    LX_STATE_Y_COMMENT, // PUNCT
    LX_STATE_Y_COMMENT, // WHITESPACE
  },
  [LX_STATE_Y_START] = {
    LX_STATE_ID, // ALPHA
    LX_STATE_NUMBER, // DIGIT
    LX_STATE_Y_COMMENT, // HASH
    LX_STATE_Z_ERROR, // ILLEGAL
    LX_STATE_MINUS, // MINUS
    LX_STATE_Y_START, // NEWLINE
    LX_STATE_Z_EOF, // NULL
    LX_STATE_OP, // OP
    LX_STATE_Z_PUNCT, // PUNCT
    LX_STATE_Y_START, // WHITESPACE
  },
};

static lx_token_t (* lx_jump_table[LX_STATE_COUNT])(char *, lx_state_t, i64) = {
  [LX_STATE_ID] = lx_step__loop,
  [LX_STATE_MINUS] = lx_step__loop,
  [LX_STATE_NUMBER] = lx_step__loop,
  [LX_STATE_OP] = lx_step__loop,
  [LX_STATE_Y_COMMENT] = lx_step__loop,
  [LX_STATE_Y_START] = lx_step__loop,
  [LX_STATE_Z_EOF] = lx_step__eof,
  [LX_STATE_Z_ERROR] = lx_step__error,
  [LX_STATE_Z_ID] = lx_step__id,
  [LX_STATE_Z_NUMBER] = lx_step__number,
  [LX_STATE_Z_OP] = lx_step__op,
  [LX_STATE_Z_PUNCT] = lx_step__punct,
};
