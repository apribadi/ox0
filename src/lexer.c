// lexing
//
// Lexer lexer_make(char const * start)
//
// Token lexer_next(Lexer * t)

typedef enum : u8 {
  // punctuation

  TOKEN_ASSIGNMENT,
  TOKEN_COMMA,
  TOKEN_DOT,
  TOKEN_COLON,
  TOKEN_SEMICOLON,
  TOKEN_LBRACE,
  TOKEN_RBRACE,
  TOKEN_LBRACKET,
  TOKEN_RBRACKET,
  TOKEN_LPAREN,
  TOKEN_RPAREN,

  // operator

  TOKEN_ADD,
  TOKEN_SUB,
  TOKEN_MUL,
  TOKEN_DIV,
  TOKEN_NEG,
  TOKEN_EQ,
  TOKEN_NE,
  TOKEN_GE,
  TOKEN_GT,
  TOKEN_LE,
  TOKEN_LT,

  // keyword

  TOKEN_AND,
  TOKEN_CASE,
  TOKEN_DO,
  TOKEN_ELIF,
  TOKEN_ELSE,
  TOKEN_END,
  TOKEN_FOR,
  TOKEN_FUNCTION,
  TOKEN_IF,
  TOKEN_LET,
  TOKEN_OR,
  TOKEN_THEN,
  TOKEN_WHILE,

  // misc

  TOKEN_IDENTIFIER,
  TOKEN_NUMBER,
  TOKEN_EOF,
  TOKEN_ILLEGAL_CHARACTER,
} TokenTag;

static char const * token_tag_name(TokenTag t) {
  switch (t) {
    case TOKEN_ASSIGNMENT: return "ASSIGNMENT";
    case TOKEN_COMMA: return "COMMA";
    case TOKEN_DOT: return "DOT";
    case TOKEN_COLON: return "COLON";
    case TOKEN_SEMICOLON: return "SEMICOLON";
    case TOKEN_LBRACE: return "LBRACE";
    case TOKEN_RBRACE: return "RBRACE";
    case TOKEN_LBRACKET: return "LBRACKET";
    case TOKEN_RBRACKET: return "RBRACKET";
    case TOKEN_LPAREN: return "LPAREN";
    case TOKEN_RPAREN: return "RPAREN";
    case TOKEN_ADD: return "ADD";
    case TOKEN_SUB: return "SUB";
    case TOKEN_MUL: return "MUL";
    case TOKEN_DIV: return "DIV";
    case TOKEN_NEG: return "NEG";
    case TOKEN_EQ: return "EQ";
    case TOKEN_NE: return "NE";
    case TOKEN_GE: return "GE";
    case TOKEN_GT: return "GT";
    case TOKEN_LE: return "LE";
    case TOKEN_LT: return "LT";
    case TOKEN_AND: return "AND";
    case TOKEN_CASE: return "CASE";
    case TOKEN_DO: return "DO";
    case TOKEN_ELIF: return "ELIF";
    case TOKEN_ELSE: return "ELSE";
    case TOKEN_END: return "END";
    case TOKEN_FOR: return "FOR";
    case TOKEN_FUNCTION: return "FUNCTION";
    case TOKEN_IF: return "IF";
    case TOKEN_LET: return "LET";
    case TOKEN_OR: return "OR";
    case TOKEN_THEN: return "THEN";
    case TOKEN_WHILE: return "WHILE";
    case TOKEN_IDENTIFIER: return "ID";
    case TOKEN_NUMBER: return "NUMBER";
    case TOKEN_EOF: return "EOF";
    case TOKEN_ILLEGAL_CHARACTER: return "ILLEGAL_CHARACTER";
  }

  return "UNKNOWN";
}

// token

typedef struct {
  TokenTag tag;
  char const * start;
  char const * stop;
} Token;

static inline Token token_make(TokenTag tag, char const * start, char const * stop) {
  return (Token) { .tag = tag, .start = start, .stop = stop };
}

static void token_show(Token t) {
  printf("%s: %.*s\n", token_tag_name(t.tag), (int) (t.stop - t.start), t.start);
}

typedef struct {
  char const * pos;
} Lexer;

static inline Lexer lexer_make(char const * start) {
  // Input string MUST be terminated by a '\0' character.

  return (Lexer) { .pos = start };
}

typedef enum : u8 {
  LEXER_CLASS_UNUSED = 0,
  LEXER_CLASS_ALPHA,
  LEXER_CLASS_DIGIT,
  LEXER_CLASS_HASH,
  LEXER_CLASS_MINUS,
  LEXER_CLASS_NEWLINE,
  LEXER_CLASS_NULL,
  LEXER_CLASS_OPERATOR,
  LEXER_CLASS_PUNCT,
  LEXER_CLASS_WHITESPACE,
} LexerCharacterClass;

#define LEXER_CLASS_COUNT 10

typedef enum : u8 {
  LEXER_STATE_IDENTIFIER,
  LEXER_STATE_MINUS,
  LEXER_STATE_NUMBER,
  LEXER_STATE_OPERATOR,
  LEXER_STATE_COMMENT,
  LEXER_STATE_START,
  LEXER_STATE_COMPLETE_IDENTIFIER,
  LEXER_STATE_COMPLETE_NUMBER,
  LEXER_STATE_COMPLETE_OPERATOR,
  LEXER_STATE_EOF,
  LEXER_STATE_ILLEGAL_CHARACTER,
  LEXER_STATE_PUNCT,
} LexerState;

#define LEXER_STATE_COUNT_MULTICHAR_TOKEN 4
#define LEXER_STATE_COUNT_NONTERMINAL 6
#define LEXER_STATE_COUNT 12

static LexerCharacterClass const lexer_character_class_table[256];

static LexerState const lexer_transition_table[LEXER_STATE_COUNT_NONTERMINAL][LEXER_CLASS_COUNT];

static Token (* const lexer_jump_table[LEXER_STATE_COUNT])(LexerState, char const *, i64);

static Token lexer_next__loop(LexerState s, char const * p, i64 n) {
  n = n + (s < LEXER_STATE_COUNT_MULTICHAR_TOKEN);
  s = lexer_transition_table[s][lexer_character_class_table[(u8) * p]];
  p = p + 1;

  return lexer_jump_table[s](s, p, n);
}

static inline Token lexer_next(Lexer * t) {
  Token r = lexer_next__loop(LEXER_STATE_START, t->pos, 0);
  t->pos = r.stop;
  return r;
}

static Token lexer_next__eof(__unused LexerState s, char const * p, __unused i64 n) {
  char const * start = p - 1;
  char const * stop = p - 1;

  return token_make(TOKEN_EOF, start, stop);
}

static Token lexer_next__illegal_character(__unused LexerState s, char const * p, __unused i64 n) {
  char const * start = p - 1;
  char const * stop = p;

  return token_make(TOKEN_ILLEGAL_CHARACTER, start, stop);
}

static Token lexer_next__identifier(__unused LexerState s, char const * p, i64 n) {
  char const * start = p - 1 - n;
  char const * stop = p - 1;

  switch (n) {
    case 2:
      if (!bcmp(start, "do", 2)) return token_make(TOKEN_DO, start, stop);
      if (!bcmp(start, "if", 2)) return token_make(TOKEN_IF, start, stop);
      if (!bcmp(start, "or", 2)) return token_make(TOKEN_OR, start, stop);
      break;
    case 3:
      if (!bcmp(start, "and", 3)) return token_make(TOKEN_AND, start, stop);
      if (!bcmp(start, "end", 3)) return token_make(TOKEN_END, start, stop);
      if (!bcmp(start, "for", 3)) return token_make(TOKEN_FOR, start, stop);
      if (!bcmp(start, "let", 3)) return token_make(TOKEN_LET, start, stop);
      break;
    case 4:
      if (!bcmp(start, "case", 4)) return token_make(TOKEN_CASE, start, stop);
      if (!bcmp(start, "elif", 4)) return token_make(TOKEN_ELIF, start, stop);
      if (!bcmp(start, "else", 4)) return token_make(TOKEN_ELSE, start, stop);
      if (!bcmp(start, "then", 4)) return token_make(TOKEN_THEN, start, stop);
      break;
    case 5:
      if (!bcmp(start, "while", 5)) return token_make(TOKEN_WHILE, start, stop);
      break;
    case 8:
      if (!bcmp(start, "function", 8)) return token_make(TOKEN_FUNCTION, start, stop);
      break;
  }

  return token_make(TOKEN_IDENTIFIER, start, stop);
}

static Token lexer_next__number(__unused LexerState s, char const * p, i64 n) {
  char const * start = p - 1 - n;
  char const * stop = p - 1;

  return token_make(TOKEN_NUMBER, start, stop);
}

static Token lexer_next__operator(__unused LexerState s, char const * p, i64 n) {
  char const * start = p - 1 - n;
  char const * stop = p - 1;

  switch (n) {
    case 1:
      switch (* start) {
        case '=': return token_make(TOKEN_ASSIGNMENT, start, stop);
        case '+': return token_make(TOKEN_ADD, start, stop);
        case '-': return token_make(TOKEN_SUB, start, stop);
        case '*': return token_make(TOKEN_MUL, start, stop);
        case '~': return token_make(TOKEN_NEG, start, stop);
        case '/': return token_make(TOKEN_DIV, start, stop);
        case '<': return token_make(TOKEN_LT, start, stop);
        case '>': return token_make(TOKEN_GT, start, stop);
      }
      break;
    case 2:
      if (* (start + 1) == '=') {
        switch (* start) {
          case '=': return token_make(TOKEN_EQ, start, stop);
          case '!': return token_make(TOKEN_NE, start, stop);
          case '<': return token_make(TOKEN_LE, start, stop);
          case '>': return token_make(TOKEN_GE, start, stop);
        }
      }
      break;
  };

  return token_make(TOKEN_ILLEGAL_CHARACTER, start, stop);
}

static Token lexer_next__punct(__unused LexerState s, char const * p, __unused i64 n) {
  char const * start = p - 1;
  char const * stop = p;

  switch (* start) {
    case ',': return token_make(TOKEN_COMMA, start, stop);
    case '.': return token_make(TOKEN_DOT, start, stop);
    case ':': return token_make(TOKEN_COLON, start, stop);
    case ';': return token_make(TOKEN_SEMICOLON, start, stop);
    case '{': return token_make(TOKEN_LBRACE, start, stop);
    case '}': return token_make(TOKEN_RBRACE, start, stop);
    case '[': return token_make(TOKEN_LBRACKET, start, stop);
    case ']': return token_make(TOKEN_RBRACKET, start, stop);
    case '(': return token_make(TOKEN_LPAREN, start, stop);
    case ')': return token_make(TOKEN_RPAREN, start, stop);
  };

  // impossible

  return token_make(TOKEN_ILLEGAL_CHARACTER, start, stop);
}

static LexerCharacterClass const lexer_character_class_table[256] = {
  ['\0'] = LEXER_CLASS_NULL,
  ['\t'] = LEXER_CLASS_WHITESPACE,
  ['\n'] = LEXER_CLASS_NEWLINE,
  [' '] = LEXER_CLASS_WHITESPACE,
  ['!'] = LEXER_CLASS_OPERATOR,
  ['#'] = LEXER_CLASS_HASH,
  ['$'] = LEXER_CLASS_OPERATOR,
  ['%'] = LEXER_CLASS_OPERATOR,
  ['&'] = LEXER_CLASS_OPERATOR,
  ['('] = LEXER_CLASS_PUNCT,
  [')'] = LEXER_CLASS_PUNCT,
  ['*'] = LEXER_CLASS_OPERATOR,
  ['+'] = LEXER_CLASS_OPERATOR,
  [','] = LEXER_CLASS_PUNCT,
  ['-'] = LEXER_CLASS_MINUS,
  ['.'] = LEXER_CLASS_PUNCT,
  ['/'] = LEXER_CLASS_OPERATOR,
  ['0'] = LEXER_CLASS_DIGIT,
  ['1'] = LEXER_CLASS_DIGIT,
  ['2'] = LEXER_CLASS_DIGIT,
  ['3'] = LEXER_CLASS_DIGIT,
  ['4'] = LEXER_CLASS_DIGIT,
  ['5'] = LEXER_CLASS_DIGIT,
  ['6'] = LEXER_CLASS_DIGIT,
  ['7'] = LEXER_CLASS_DIGIT,
  ['8'] = LEXER_CLASS_DIGIT,
  ['9'] = LEXER_CLASS_DIGIT,
  [':'] = LEXER_CLASS_PUNCT,
  [';'] = LEXER_CLASS_PUNCT,
  ['<'] = LEXER_CLASS_OPERATOR,
  ['='] = LEXER_CLASS_OPERATOR,
  ['>'] = LEXER_CLASS_OPERATOR,
  ['?'] = LEXER_CLASS_OPERATOR,
  ['@'] = LEXER_CLASS_OPERATOR,
  ['A'] = LEXER_CLASS_ALPHA,
  ['B'] = LEXER_CLASS_ALPHA,
  ['C'] = LEXER_CLASS_ALPHA,
  ['D'] = LEXER_CLASS_ALPHA,
  ['E'] = LEXER_CLASS_ALPHA,
  ['F'] = LEXER_CLASS_ALPHA,
  ['G'] = LEXER_CLASS_ALPHA,
  ['H'] = LEXER_CLASS_ALPHA,
  ['I'] = LEXER_CLASS_ALPHA,
  ['J'] = LEXER_CLASS_ALPHA,
  ['K'] = LEXER_CLASS_ALPHA,
  ['L'] = LEXER_CLASS_ALPHA,
  ['M'] = LEXER_CLASS_ALPHA,
  ['N'] = LEXER_CLASS_ALPHA,
  ['O'] = LEXER_CLASS_ALPHA,
  ['P'] = LEXER_CLASS_ALPHA,
  ['Q'] = LEXER_CLASS_ALPHA,
  ['R'] = LEXER_CLASS_ALPHA,
  ['S'] = LEXER_CLASS_ALPHA,
  ['T'] = LEXER_CLASS_ALPHA,
  ['U'] = LEXER_CLASS_ALPHA,
  ['V'] = LEXER_CLASS_ALPHA,
  ['W'] = LEXER_CLASS_ALPHA,
  ['X'] = LEXER_CLASS_ALPHA,
  ['Y'] = LEXER_CLASS_ALPHA,
  ['Z'] = LEXER_CLASS_ALPHA,
  ['['] = LEXER_CLASS_PUNCT,
  [']'] = LEXER_CLASS_PUNCT,
  ['^'] = LEXER_CLASS_OPERATOR,
  ['_'] = LEXER_CLASS_ALPHA,
  ['a'] = LEXER_CLASS_ALPHA,
  ['b'] = LEXER_CLASS_ALPHA,
  ['c'] = LEXER_CLASS_ALPHA,
  ['d'] = LEXER_CLASS_ALPHA,
  ['e'] = LEXER_CLASS_ALPHA,
  ['f'] = LEXER_CLASS_ALPHA,
  ['g'] = LEXER_CLASS_ALPHA,
  ['h'] = LEXER_CLASS_ALPHA,
  ['i'] = LEXER_CLASS_ALPHA,
  ['j'] = LEXER_CLASS_ALPHA,
  ['k'] = LEXER_CLASS_ALPHA,
  ['l'] = LEXER_CLASS_ALPHA,
  ['m'] = LEXER_CLASS_ALPHA,
  ['n'] = LEXER_CLASS_ALPHA,
  ['o'] = LEXER_CLASS_ALPHA,
  ['p'] = LEXER_CLASS_ALPHA,
  ['q'] = LEXER_CLASS_ALPHA,
  ['r'] = LEXER_CLASS_ALPHA,
  ['s'] = LEXER_CLASS_ALPHA,
  ['t'] = LEXER_CLASS_ALPHA,
  ['u'] = LEXER_CLASS_ALPHA,
  ['v'] = LEXER_CLASS_ALPHA,
  ['w'] = LEXER_CLASS_ALPHA,
  ['x'] = LEXER_CLASS_ALPHA,
  ['y'] = LEXER_CLASS_ALPHA,
  ['z'] = LEXER_CLASS_ALPHA,
  ['{'] = LEXER_CLASS_PUNCT,
  ['|'] = LEXER_CLASS_OPERATOR,
  ['}'] = LEXER_CLASS_PUNCT,
  ['~'] = LEXER_CLASS_OPERATOR,
};

static LexerState const lexer_transition_table[LEXER_STATE_COUNT_NONTERMINAL][LEXER_CLASS_COUNT] = {
  [LEXER_STATE_IDENTIFIER] = {
    [LEXER_CLASS_UNUSED]     = LEXER_STATE_COMPLETE_IDENTIFIER,
    [LEXER_CLASS_ALPHA]      = LEXER_STATE_IDENTIFIER,
    [LEXER_CLASS_DIGIT]      = LEXER_STATE_IDENTIFIER,
    [LEXER_CLASS_HASH]       = LEXER_STATE_COMPLETE_IDENTIFIER,
    [LEXER_CLASS_MINUS]      = LEXER_STATE_COMPLETE_IDENTIFIER,
    [LEXER_CLASS_NEWLINE]    = LEXER_STATE_COMPLETE_IDENTIFIER,
    [LEXER_CLASS_NULL]       = LEXER_STATE_COMPLETE_IDENTIFIER,
    [LEXER_CLASS_OPERATOR]   = LEXER_STATE_COMPLETE_IDENTIFIER,
    [LEXER_CLASS_PUNCT]      = LEXER_STATE_COMPLETE_IDENTIFIER,
    [LEXER_CLASS_WHITESPACE] = LEXER_STATE_COMPLETE_IDENTIFIER,
  },
  [LEXER_STATE_MINUS] = {
    [LEXER_CLASS_UNUSED]     = LEXER_STATE_COMPLETE_OPERATOR,
    [LEXER_CLASS_ALPHA]      = LEXER_STATE_COMPLETE_OPERATOR,
    [LEXER_CLASS_DIGIT]      = LEXER_STATE_NUMBER,
    [LEXER_CLASS_HASH]       = LEXER_STATE_COMPLETE_OPERATOR,
    [LEXER_CLASS_MINUS]      = LEXER_STATE_OPERATOR,
    [LEXER_CLASS_NEWLINE]    = LEXER_STATE_COMPLETE_OPERATOR,
    [LEXER_CLASS_NULL]       = LEXER_STATE_COMPLETE_OPERATOR,
    [LEXER_CLASS_OPERATOR]   = LEXER_STATE_OPERATOR,
    [LEXER_CLASS_PUNCT]      = LEXER_STATE_COMPLETE_OPERATOR,
    [LEXER_CLASS_WHITESPACE] = LEXER_STATE_COMPLETE_OPERATOR,
  },
  [LEXER_STATE_NUMBER] = {
    [LEXER_CLASS_UNUSED]     = LEXER_STATE_COMPLETE_NUMBER,
    [LEXER_CLASS_ALPHA]      = LEXER_STATE_COMPLETE_NUMBER,
    [LEXER_CLASS_DIGIT]      = LEXER_STATE_NUMBER,
    [LEXER_CLASS_HASH]       = LEXER_STATE_COMPLETE_NUMBER,
    [LEXER_CLASS_MINUS]      = LEXER_STATE_COMPLETE_NUMBER,
    [LEXER_CLASS_NEWLINE]    = LEXER_STATE_COMPLETE_NUMBER,
    [LEXER_CLASS_NULL]       = LEXER_STATE_COMPLETE_NUMBER,
    [LEXER_CLASS_OPERATOR]   = LEXER_STATE_COMPLETE_NUMBER,
    [LEXER_CLASS_PUNCT]      = LEXER_STATE_COMPLETE_NUMBER,
    [LEXER_CLASS_WHITESPACE] = LEXER_STATE_COMPLETE_NUMBER,
  },
  [LEXER_STATE_OPERATOR] = {
    [LEXER_CLASS_UNUSED]     = LEXER_STATE_COMPLETE_OPERATOR,
    [LEXER_CLASS_ALPHA]      = LEXER_STATE_COMPLETE_OPERATOR,
    [LEXER_CLASS_DIGIT]      = LEXER_STATE_COMPLETE_OPERATOR,
    [LEXER_CLASS_HASH]       = LEXER_STATE_COMPLETE_OPERATOR,
    [LEXER_CLASS_MINUS]      = LEXER_STATE_OPERATOR,
    [LEXER_CLASS_NEWLINE]    = LEXER_STATE_COMPLETE_OPERATOR,
    [LEXER_CLASS_NULL]       = LEXER_STATE_COMPLETE_OPERATOR,
    [LEXER_CLASS_OPERATOR]   = LEXER_STATE_OPERATOR,
    [LEXER_CLASS_PUNCT]      = LEXER_STATE_COMPLETE_OPERATOR,
    [LEXER_CLASS_WHITESPACE] = LEXER_STATE_COMPLETE_OPERATOR,
  },
  [LEXER_STATE_COMMENT] = {
    [LEXER_CLASS_UNUSED]     = LEXER_STATE_COMMENT,
    [LEXER_CLASS_ALPHA]      = LEXER_STATE_COMMENT,
    [LEXER_CLASS_DIGIT]      = LEXER_STATE_COMMENT,
    [LEXER_CLASS_HASH]       = LEXER_STATE_COMMENT,
    [LEXER_CLASS_MINUS]      = LEXER_STATE_COMMENT,
    [LEXER_CLASS_NEWLINE]    = LEXER_STATE_START,
    [LEXER_CLASS_NULL]       = LEXER_STATE_EOF,
    [LEXER_CLASS_OPERATOR]   = LEXER_STATE_COMMENT,
    [LEXER_CLASS_PUNCT]      = LEXER_STATE_COMMENT,
    [LEXER_CLASS_WHITESPACE] = LEXER_STATE_COMMENT,
  },
  [LEXER_STATE_START] = {
    [LEXER_CLASS_UNUSED]     = LEXER_STATE_ILLEGAL_CHARACTER,
    [LEXER_CLASS_ALPHA]      = LEXER_STATE_IDENTIFIER,
    [LEXER_CLASS_DIGIT]      = LEXER_STATE_NUMBER,
    [LEXER_CLASS_HASH]       = LEXER_STATE_COMMENT,
    [LEXER_CLASS_MINUS]      = LEXER_STATE_MINUS,
    [LEXER_CLASS_NEWLINE]    = LEXER_STATE_START,
    [LEXER_CLASS_NULL]       = LEXER_STATE_EOF,
    [LEXER_CLASS_OPERATOR]   = LEXER_STATE_OPERATOR,
    [LEXER_CLASS_PUNCT]      = LEXER_STATE_PUNCT,
    [LEXER_CLASS_WHITESPACE] = LEXER_STATE_START,
  },
};

static Token (* const lexer_jump_table[LEXER_STATE_COUNT])(LexerState, char const *, i64) = {
  [LEXER_STATE_IDENTIFIER]          = lexer_next__loop,
  [LEXER_STATE_MINUS]               = lexer_next__loop,
  [LEXER_STATE_NUMBER]              = lexer_next__loop,
  [LEXER_STATE_OPERATOR]            = lexer_next__loop,
  [LEXER_STATE_COMMENT]             = lexer_next__loop,
  [LEXER_STATE_START]               = lexer_next__loop,
  [LEXER_STATE_COMPLETE_IDENTIFIER] = lexer_next__identifier,
  [LEXER_STATE_COMPLETE_NUMBER]     = lexer_next__number,
  [LEXER_STATE_COMPLETE_OPERATOR]   = lexer_next__operator,
  [LEXER_STATE_EOF]                 = lexer_next__eof,
  [LEXER_STATE_ILLEGAL_CHARACTER]   = lexer_next__illegal_character,
  [LEXER_STATE_PUNCT]               = lexer_next__punct,
};
