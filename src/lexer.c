// lexing
//
// lx_t
//
// lx_t lx_make(char const * source)
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

  // misc

  LX_TOKEN_ID,
  LX_TOKEN_NUMBER,
  LX_TOKEN_EOF,
  LX_TOKEN_ERROR,
};

typedef u8 lx_token_tag_t;

static char const * lx_token_tag_name(lx_token_tag_t t) {
  switch (t) {
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

// token

typedef struct {
  lx_token_tag_t tag;
  char const * start;
  char const * stop;
} lx_token_t;

static inline lx_token_t lx_token_make(lx_token_tag_t tag, char const * start, char const * stop) {
  return (lx_token_t) { .tag = tag, .start = start, .stop = stop };
}

static void lx_token_show(lx_token_t t) {
  printf("%s: %.*s\n", lx_token_tag_name(t.tag), (int) (t.stop - t.start), t.start);
}

typedef struct {
  char const * pos;
} lx_t;

static inline lx_t lx_make(char const * source) {
  // Source MUST be terminated by a '\0' character.

  return (lx_t) { .pos = source };
}

// character class

enum {
  LX_CLASS_UNUSED = 0,
  LX_CLASS_ALPHA,
  LX_CLASS_DIGIT,
  LX_CLASS_HASH,
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

  LX_STATE_COMMENT,
  LX_STATE_START,

  LX_STATE_COMPLETE_ID,
  LX_STATE_COMPLETE_NUMBER,
  LX_STATE_COMPLETE_OP,
  LX_STATE_EOF,
  LX_STATE_ERROR,
  LX_STATE_PUNCT,

  LX_STATE_COUNT,
};

#define LX_STATE_INCLUDED_COUNT 4

typedef u8 lx_state_t;

static lx_class_t lx_class_table[];

static lx_state_t lx_transition_table[][LX_CLASS_COUNT];

static lx_token_t (* lx_jump_table[])(char const *, lx_state_t, i64);

static lx_token_t lx_step__loop(char const * p, lx_state_t s, i64 n) {
  n = n + (s < LX_STATE_INCLUDED_COUNT);
  s = lx_transition_table[s][lx_class_table[(u8) * p]];
  p = p + 1;

  return lx_jump_table[s](p, s, n);
}

static inline lx_token_t lx_step(lx_t * t) {
  char const * p = t->pos;
  lx_token_t r = lx_step__loop(p, LX_STATE_START, 0);
  t->pos = r.stop;
  return r;
}

static lx_token_t lx_step__eof(char const * p, __attribute__((unused)) lx_state_t s, __attribute__((unused)) i64 n) {
  char const * start = p - 1;
  char const * stop = p - 1;

  return lx_token_make(LX_TOKEN_EOF, start, stop);
}

static lx_token_t lx_step__error(char const * p, __attribute__((unused)) lx_state_t s, i64 n) {
  char const * start = p - 1 - n;
  char const * stop = p;

  return lx_token_make(LX_TOKEN_ERROR, start, stop);
}

static lx_token_t lx_step__id(char const * p, __attribute__((unused)) lx_state_t s, i64 n) {
  char const * start = p - 1 - n;
  char const * stop = p - 1;

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

static lx_token_t lx_step__number(char const * p, __attribute__((unused)) lx_state_t s, i64 n) {
  char const * start = p - 1 - n;
  char const * stop = p - 1;

  return lx_token_make(LX_TOKEN_NUMBER, start, stop);
}

static lx_token_t lx_step__op(char const * p, __attribute__((unused)) lx_state_t s, i64 n) {
  char const * start = p - 1 - n;
  char const * stop = p - 1;

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

static lx_token_t lx_step__punct(char const * p, __attribute__((unused)) lx_state_t s, __attribute__((unused)) i64 n) {
  char const * start = p - 1;
  char const * stop = p;

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

static lx_class_t lx_class_table[256] = {
  ['\0'] = LX_CLASS_NULL, // \0
  ['\t'] = LX_CLASS_WHITESPACE, // \t
  ['\n'] = LX_CLASS_NEWLINE, // \n
  [' '] = LX_CLASS_WHITESPACE,
  ['!'] = LX_CLASS_OP,
  ['#'] = LX_CLASS_HASH,
  ['$'] = LX_CLASS_OP,
  ['%'] = LX_CLASS_OP,
  ['&'] = LX_CLASS_OP,
  ['('] = LX_CLASS_PUNCT,
  [')'] = LX_CLASS_PUNCT,
  ['*'] = LX_CLASS_OP,
  ['+'] = LX_CLASS_OP,
  [','] = LX_CLASS_PUNCT,
  ['-'] = LX_CLASS_MINUS,
  ['.'] = LX_CLASS_PUNCT,
  ['/'] = LX_CLASS_OP,
  ['0'] = LX_CLASS_DIGIT,
  ['1'] = LX_CLASS_DIGIT,
  ['2'] = LX_CLASS_DIGIT,
  ['3'] = LX_CLASS_DIGIT,
  ['4'] = LX_CLASS_DIGIT,
  ['5'] = LX_CLASS_DIGIT,
  ['6'] = LX_CLASS_DIGIT,
  ['7'] = LX_CLASS_DIGIT,
  ['8'] = LX_CLASS_DIGIT,
  ['9'] = LX_CLASS_DIGIT,
  [':'] = LX_CLASS_PUNCT,
  [';'] = LX_CLASS_PUNCT,
  ['<'] = LX_CLASS_OP,
  ['='] = LX_CLASS_OP,
  ['>'] = LX_CLASS_OP,
  ['?'] = LX_CLASS_OP,
  ['@'] = LX_CLASS_OP,
  ['A'] = LX_CLASS_ALPHA,
  ['B'] = LX_CLASS_ALPHA,
  ['C'] = LX_CLASS_ALPHA,
  ['D'] = LX_CLASS_ALPHA,
  ['E'] = LX_CLASS_ALPHA,
  ['F'] = LX_CLASS_ALPHA,
  ['G'] = LX_CLASS_ALPHA,
  ['H'] = LX_CLASS_ALPHA,
  ['I'] = LX_CLASS_ALPHA,
  ['J'] = LX_CLASS_ALPHA,
  ['K'] = LX_CLASS_ALPHA,
  ['L'] = LX_CLASS_ALPHA,
  ['M'] = LX_CLASS_ALPHA,
  ['N'] = LX_CLASS_ALPHA,
  ['O'] = LX_CLASS_ALPHA,
  ['P'] = LX_CLASS_ALPHA,
  ['Q'] = LX_CLASS_ALPHA,
  ['R'] = LX_CLASS_ALPHA,
  ['S'] = LX_CLASS_ALPHA,
  ['T'] = LX_CLASS_ALPHA,
  ['U'] = LX_CLASS_ALPHA,
  ['V'] = LX_CLASS_ALPHA,
  ['W'] = LX_CLASS_ALPHA,
  ['X'] = LX_CLASS_ALPHA,
  ['Y'] = LX_CLASS_ALPHA,
  ['Z'] = LX_CLASS_ALPHA,
  ['['] = LX_CLASS_PUNCT,
  [']'] = LX_CLASS_PUNCT,
  ['^'] = LX_CLASS_OP,
  ['_'] = LX_CLASS_ALPHA,
  ['a'] = LX_CLASS_ALPHA,
  ['b'] = LX_CLASS_ALPHA,
  ['c'] = LX_CLASS_ALPHA,
  ['d'] = LX_CLASS_ALPHA,
  ['e'] = LX_CLASS_ALPHA,
  ['f'] = LX_CLASS_ALPHA,
  ['g'] = LX_CLASS_ALPHA,
  ['h'] = LX_CLASS_ALPHA,
  ['i'] = LX_CLASS_ALPHA,
  ['j'] = LX_CLASS_ALPHA,
  ['k'] = LX_CLASS_ALPHA,
  ['l'] = LX_CLASS_ALPHA,
  ['m'] = LX_CLASS_ALPHA,
  ['n'] = LX_CLASS_ALPHA,
  ['o'] = LX_CLASS_ALPHA,
  ['p'] = LX_CLASS_ALPHA,
  ['q'] = LX_CLASS_ALPHA,
  ['r'] = LX_CLASS_ALPHA,
  ['s'] = LX_CLASS_ALPHA,
  ['t'] = LX_CLASS_ALPHA,
  ['u'] = LX_CLASS_ALPHA,
  ['v'] = LX_CLASS_ALPHA,
  ['w'] = LX_CLASS_ALPHA,
  ['x'] = LX_CLASS_ALPHA,
  ['y'] = LX_CLASS_ALPHA,
  ['z'] = LX_CLASS_ALPHA,
  ['{'] = LX_CLASS_PUNCT,
  ['|'] = LX_CLASS_OP,
  ['}'] = LX_CLASS_PUNCT,
  ['~'] = LX_CLASS_OP,
};

static lx_state_t lx_transition_table[][LX_CLASS_COUNT] = {
  [LX_STATE_ID] = {
    LX_STATE_COMPLETE_ID, // UNUSED
    LX_STATE_ID, // ALPHA
    LX_STATE_ID, // DIGIT
    LX_STATE_COMPLETE_ID, // HASH
    LX_STATE_COMPLETE_ID, // MINUS
    LX_STATE_COMPLETE_ID, // NEWLINE
    LX_STATE_COMPLETE_ID, // NULL
    LX_STATE_COMPLETE_ID, // OP
    LX_STATE_COMPLETE_ID, // PUNCT
    LX_STATE_COMPLETE_ID, // WHITESPACE
  },
  [LX_STATE_MINUS] = {
    LX_STATE_COMPLETE_OP, // UNUSED
    LX_STATE_COMPLETE_OP, // ALPHA
    LX_STATE_NUMBER, // DIGIT
    LX_STATE_COMPLETE_OP, // HASH
    LX_STATE_OP, // MINUS
    LX_STATE_COMPLETE_OP, // NEWLINE
    LX_STATE_COMPLETE_OP, // NULL
    LX_STATE_OP, // OP
    LX_STATE_COMPLETE_OP, // PUNCT
    LX_STATE_COMPLETE_OP, // WHITESPACE
  },
  [LX_STATE_NUMBER] = {
    LX_STATE_COMPLETE_NUMBER, // UNUSED
    LX_STATE_COMPLETE_NUMBER, // ALPHA
    LX_STATE_NUMBER, // DIGIT
    LX_STATE_COMPLETE_NUMBER, // HASH
    LX_STATE_COMPLETE_NUMBER, // MINUS
    LX_STATE_COMPLETE_NUMBER, // NEWLINE
    LX_STATE_COMPLETE_NUMBER, // NULL
    LX_STATE_COMPLETE_NUMBER, // OP
    LX_STATE_COMPLETE_NUMBER, // PUNCT
    LX_STATE_COMPLETE_NUMBER, // WHITESPACE
  },
  [LX_STATE_OP] = {
    LX_STATE_COMPLETE_OP, // UNUSED
    LX_STATE_COMPLETE_OP, // ALPHA
    LX_STATE_COMPLETE_OP, // DIGIT
    LX_STATE_COMPLETE_OP, // HASH
    LX_STATE_OP, // MINUS
    LX_STATE_COMPLETE_OP, // NEWLINE
    LX_STATE_COMPLETE_OP, // NULL
    LX_STATE_OP, // OP
    LX_STATE_COMPLETE_OP, // PUNCT
    LX_STATE_COMPLETE_OP, // WHITESPACE
  },
  [LX_STATE_COMMENT] = {
    LX_STATE_COMMENT, // UNUSED
    LX_STATE_COMMENT, // ALPHA
    LX_STATE_COMMENT, // DIGIT
    LX_STATE_COMMENT, // HASH
    LX_STATE_COMMENT, // MINUS
    LX_STATE_START, // NEWLINE
    LX_STATE_EOF, // NULL
    LX_STATE_COMMENT, // OP
    LX_STATE_COMMENT, // PUNCT
    LX_STATE_COMMENT, // WHITESPACE
  },
  [LX_STATE_START] = {
    LX_STATE_ERROR, // UNUSED
    LX_STATE_ID, // ALPHA
    LX_STATE_NUMBER, // DIGIT
    LX_STATE_COMMENT, // HASH
    LX_STATE_MINUS, // MINUS
    LX_STATE_START, // NEWLINE
    LX_STATE_EOF, // NULL
    LX_STATE_OP, // OP
    LX_STATE_PUNCT, // PUNCT
    LX_STATE_START, // WHITESPACE
  },
};

static lx_token_t (* lx_jump_table[LX_STATE_COUNT])(char const *, lx_state_t, i64) = {
  [LX_STATE_ID] = lx_step__loop,
  [LX_STATE_MINUS] = lx_step__loop,
  [LX_STATE_NUMBER] = lx_step__loop,
  [LX_STATE_OP] = lx_step__loop,
  [LX_STATE_COMMENT] = lx_step__loop,
  [LX_STATE_START] = lx_step__loop,
  [LX_STATE_COMPLETE_ID] = lx_step__id,
  [LX_STATE_COMPLETE_NUMBER] = lx_step__number,
  [LX_STATE_COMPLETE_OP] = lx_step__op,
  [LX_STATE_EOF] = lx_step__eof,
  [LX_STATE_ERROR] = lx_step__error,
  [LX_STATE_PUNCT] = lx_step__punct,
};
