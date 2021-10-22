// lexing
//
// Token next_token(char const * position)

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

static char const * token_name(TokenTag t) {
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
}

// token

typedef struct {
  TokenTag tag;
  char const * start;
  char const * stop;
} Token;

static inline Token make_token(TokenTag tag, char const * start, char const * stop) {
  return (Token) { .tag = tag, .start = start, .stop = stop };
}

static Symbol token_symbol(Token t) {
  return make_symbol(t.start, t.stop);
}

typedef enum : u8 {
  CHARACTER_CLASS_UNUSED = 0,
  CHARACTER_CLASS_ALPHA,
  CHARACTER_CLASS_DIGIT,
  CHARACTER_CLASS_HASH,
  CHARACTER_CLASS_MINUS,
  CHARACTER_CLASS_NEWLINE,
  CHARACTER_CLASS_NULL,
  CHARACTER_CLASS_OPERATOR,
  CHARACTER_CLASS_PUNCT,
  CHARACTER_CLASS_WHITESPACE,
} CharacterClass;

#define CHARACTER_CLASS_COUNT 10

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

static CharacterClass const character_class_table[256];

static LexerState const lexer_transition_table[LEXER_STATE_COUNT_NONTERMINAL][CHARACTER_CLASS_COUNT];

static Token (* const lexer_jump_table[LEXER_STATE_COUNT])(LexerState, char const *, i64);

static Token next_token__loop(LexerState s, char const * p, i64 n) {
  n = n + (s < LEXER_STATE_COUNT_MULTICHAR_TOKEN);
  s = lexer_transition_table[s][character_class_table[(u8) * p]];
  p = p + 1;

  return lexer_jump_table[s](s, p, n);
}

static inline Token next_token(char const * position) {
  return next_token__loop(LEXER_STATE_START, position, 0);
}

static Token next_token__eof(__unused LexerState s, char const * p, __unused i64 n) {
  char const * start = p - 1;
  char const * stop = p - 1;

  return make_token(TOKEN_EOF, start, stop);
}

static Token next_token__illegal_character(__unused LexerState s, char const * p, __unused i64 n) {
  char const * start = p - 1;
  char const * stop = p;

  return make_token(TOKEN_ILLEGAL_CHARACTER, start, stop);
}

static Token next_token__identifier(__unused LexerState s, char const * p, i64 n) {
  char const * start = p - 1 - n;
  char const * stop = p - 1;

  switch (n) {
    case 2:
      if (!bcmp(start, "do", 2)) return make_token(TOKEN_DO, start, stop);
      if (!bcmp(start, "if", 2)) return make_token(TOKEN_IF, start, stop);
      if (!bcmp(start, "or", 2)) return make_token(TOKEN_OR, start, stop);
      break;
    case 3:
      if (!bcmp(start, "and", 3)) return make_token(TOKEN_AND, start, stop);
      if (!bcmp(start, "end", 3)) return make_token(TOKEN_END, start, stop);
      if (!bcmp(start, "for", 3)) return make_token(TOKEN_FOR, start, stop);
      if (!bcmp(start, "let", 3)) return make_token(TOKEN_LET, start, stop);
      break;
    case 4:
      if (!bcmp(start, "case", 4)) return make_token(TOKEN_CASE, start, stop);
      if (!bcmp(start, "elif", 4)) return make_token(TOKEN_ELIF, start, stop);
      if (!bcmp(start, "else", 4)) return make_token(TOKEN_ELSE, start, stop);
      if (!bcmp(start, "then", 4)) return make_token(TOKEN_THEN, start, stop);
      break;
    case 5:
      if (!bcmp(start, "while", 5)) return make_token(TOKEN_WHILE, start, stop);
      break;
    case 8:
      if (!bcmp(start, "function", 8)) return make_token(TOKEN_FUNCTION, start, stop);
      break;
  }

  return make_token(TOKEN_IDENTIFIER, start, stop);
}

static Token next_token__number(__unused LexerState s, char const * p, i64 n) {
  char const * start = p - 1 - n;
  char const * stop = p - 1;

  return make_token(TOKEN_NUMBER, start, stop);
}

static Token next_token__operator(__unused LexerState s, char const * p, i64 n) {
  char const * start = p - 1 - n;
  char const * stop = p - 1;

  switch (n) {
    case 1:
      switch (* start) {
        case '=': return make_token(TOKEN_ASSIGNMENT, start, stop);
        case '+': return make_token(TOKEN_ADD, start, stop);
        case '-': return make_token(TOKEN_SUB, start, stop);
        case '*': return make_token(TOKEN_MUL, start, stop);
        case '~': return make_token(TOKEN_NEG, start, stop);
        case '/': return make_token(TOKEN_DIV, start, stop);
        case '<': return make_token(TOKEN_LT, start, stop);
        case '>': return make_token(TOKEN_GT, start, stop);
      }
      break;
    case 2:
      if (* (start + 1) == '=') {
        switch (* start) {
          case '=': return make_token(TOKEN_EQ, start, stop);
          case '!': return make_token(TOKEN_NE, start, stop);
          case '<': return make_token(TOKEN_LE, start, stop);
          case '>': return make_token(TOKEN_GE, start, stop);
        }
      }
      break;
  };

  return make_token(TOKEN_ILLEGAL_CHARACTER, start, stop);
}

static Token next_token__punct(__unused LexerState s, char const * p, __unused i64 n) {
  char const * start = p - 1;
  char const * stop = p;

  switch (* start) {
    case ',': return make_token(TOKEN_COMMA, start, stop);
    case '.': return make_token(TOKEN_DOT, start, stop);
    case ':': return make_token(TOKEN_COLON, start, stop);
    case ';': return make_token(TOKEN_SEMICOLON, start, stop);
    case '{': return make_token(TOKEN_LBRACE, start, stop);
    case '}': return make_token(TOKEN_RBRACE, start, stop);
    case '[': return make_token(TOKEN_LBRACKET, start, stop);
    case ']': return make_token(TOKEN_RBRACKET, start, stop);
    case '(': return make_token(TOKEN_LPAREN, start, stop);
    case ')': return make_token(TOKEN_RPAREN, start, stop);
  };

  // impossible

  return make_token(TOKEN_ILLEGAL_CHARACTER, start, stop);
}

static CharacterClass const character_class_table[256] = {
  ['\0'] = CHARACTER_CLASS_NULL,
  ['\t'] = CHARACTER_CLASS_WHITESPACE,
  ['\n'] = CHARACTER_CLASS_NEWLINE,
  [' '] = CHARACTER_CLASS_WHITESPACE,
  ['!'] = CHARACTER_CLASS_OPERATOR,
  ['#'] = CHARACTER_CLASS_HASH,
  ['$'] = CHARACTER_CLASS_OPERATOR,
  ['%'] = CHARACTER_CLASS_OPERATOR,
  ['&'] = CHARACTER_CLASS_OPERATOR,
  ['('] = CHARACTER_CLASS_PUNCT,
  [')'] = CHARACTER_CLASS_PUNCT,
  ['*'] = CHARACTER_CLASS_OPERATOR,
  ['+'] = CHARACTER_CLASS_OPERATOR,
  [','] = CHARACTER_CLASS_PUNCT,
  ['-'] = CHARACTER_CLASS_MINUS,
  ['.'] = CHARACTER_CLASS_PUNCT,
  ['/'] = CHARACTER_CLASS_OPERATOR,
  ['0'] = CHARACTER_CLASS_DIGIT,
  ['1'] = CHARACTER_CLASS_DIGIT,
  ['2'] = CHARACTER_CLASS_DIGIT,
  ['3'] = CHARACTER_CLASS_DIGIT,
  ['4'] = CHARACTER_CLASS_DIGIT,
  ['5'] = CHARACTER_CLASS_DIGIT,
  ['6'] = CHARACTER_CLASS_DIGIT,
  ['7'] = CHARACTER_CLASS_DIGIT,
  ['8'] = CHARACTER_CLASS_DIGIT,
  ['9'] = CHARACTER_CLASS_DIGIT,
  [':'] = CHARACTER_CLASS_PUNCT,
  [';'] = CHARACTER_CLASS_PUNCT,
  ['<'] = CHARACTER_CLASS_OPERATOR,
  ['='] = CHARACTER_CLASS_OPERATOR,
  ['>'] = CHARACTER_CLASS_OPERATOR,
  ['?'] = CHARACTER_CLASS_OPERATOR,
  ['@'] = CHARACTER_CLASS_OPERATOR,
  ['A'] = CHARACTER_CLASS_ALPHA,
  ['B'] = CHARACTER_CLASS_ALPHA,
  ['C'] = CHARACTER_CLASS_ALPHA,
  ['D'] = CHARACTER_CLASS_ALPHA,
  ['E'] = CHARACTER_CLASS_ALPHA,
  ['F'] = CHARACTER_CLASS_ALPHA,
  ['G'] = CHARACTER_CLASS_ALPHA,
  ['H'] = CHARACTER_CLASS_ALPHA,
  ['I'] = CHARACTER_CLASS_ALPHA,
  ['J'] = CHARACTER_CLASS_ALPHA,
  ['K'] = CHARACTER_CLASS_ALPHA,
  ['L'] = CHARACTER_CLASS_ALPHA,
  ['M'] = CHARACTER_CLASS_ALPHA,
  ['N'] = CHARACTER_CLASS_ALPHA,
  ['O'] = CHARACTER_CLASS_ALPHA,
  ['P'] = CHARACTER_CLASS_ALPHA,
  ['Q'] = CHARACTER_CLASS_ALPHA,
  ['R'] = CHARACTER_CLASS_ALPHA,
  ['S'] = CHARACTER_CLASS_ALPHA,
  ['T'] = CHARACTER_CLASS_ALPHA,
  ['U'] = CHARACTER_CLASS_ALPHA,
  ['V'] = CHARACTER_CLASS_ALPHA,
  ['W'] = CHARACTER_CLASS_ALPHA,
  ['X'] = CHARACTER_CLASS_ALPHA,
  ['Y'] = CHARACTER_CLASS_ALPHA,
  ['Z'] = CHARACTER_CLASS_ALPHA,
  ['['] = CHARACTER_CLASS_PUNCT,
  [']'] = CHARACTER_CLASS_PUNCT,
  ['^'] = CHARACTER_CLASS_OPERATOR,
  ['_'] = CHARACTER_CLASS_ALPHA,
  ['a'] = CHARACTER_CLASS_ALPHA,
  ['b'] = CHARACTER_CLASS_ALPHA,
  ['c'] = CHARACTER_CLASS_ALPHA,
  ['d'] = CHARACTER_CLASS_ALPHA,
  ['e'] = CHARACTER_CLASS_ALPHA,
  ['f'] = CHARACTER_CLASS_ALPHA,
  ['g'] = CHARACTER_CLASS_ALPHA,
  ['h'] = CHARACTER_CLASS_ALPHA,
  ['i'] = CHARACTER_CLASS_ALPHA,
  ['j'] = CHARACTER_CLASS_ALPHA,
  ['k'] = CHARACTER_CLASS_ALPHA,
  ['l'] = CHARACTER_CLASS_ALPHA,
  ['m'] = CHARACTER_CLASS_ALPHA,
  ['n'] = CHARACTER_CLASS_ALPHA,
  ['o'] = CHARACTER_CLASS_ALPHA,
  ['p'] = CHARACTER_CLASS_ALPHA,
  ['q'] = CHARACTER_CLASS_ALPHA,
  ['r'] = CHARACTER_CLASS_ALPHA,
  ['s'] = CHARACTER_CLASS_ALPHA,
  ['t'] = CHARACTER_CLASS_ALPHA,
  ['u'] = CHARACTER_CLASS_ALPHA,
  ['v'] = CHARACTER_CLASS_ALPHA,
  ['w'] = CHARACTER_CLASS_ALPHA,
  ['x'] = CHARACTER_CLASS_ALPHA,
  ['y'] = CHARACTER_CLASS_ALPHA,
  ['z'] = CHARACTER_CLASS_ALPHA,
  ['{'] = CHARACTER_CLASS_PUNCT,
  ['|'] = CHARACTER_CLASS_OPERATOR,
  ['}'] = CHARACTER_CLASS_PUNCT,
  ['~'] = CHARACTER_CLASS_OPERATOR,
};

static LexerState const lexer_transition_table[LEXER_STATE_COUNT_NONTERMINAL][CHARACTER_CLASS_COUNT] = {
  [LEXER_STATE_IDENTIFIER] = {
    [CHARACTER_CLASS_UNUSED]     = LEXER_STATE_COMPLETE_IDENTIFIER,
    [CHARACTER_CLASS_ALPHA]      = LEXER_STATE_IDENTIFIER,
    [CHARACTER_CLASS_DIGIT]      = LEXER_STATE_IDENTIFIER,
    [CHARACTER_CLASS_HASH]       = LEXER_STATE_COMPLETE_IDENTIFIER,
    [CHARACTER_CLASS_MINUS]      = LEXER_STATE_COMPLETE_IDENTIFIER,
    [CHARACTER_CLASS_NEWLINE]    = LEXER_STATE_COMPLETE_IDENTIFIER,
    [CHARACTER_CLASS_NULL]       = LEXER_STATE_COMPLETE_IDENTIFIER,
    [CHARACTER_CLASS_OPERATOR]   = LEXER_STATE_COMPLETE_IDENTIFIER,
    [CHARACTER_CLASS_PUNCT]      = LEXER_STATE_COMPLETE_IDENTIFIER,
    [CHARACTER_CLASS_WHITESPACE] = LEXER_STATE_COMPLETE_IDENTIFIER,
  },
  [LEXER_STATE_MINUS] = {
    [CHARACTER_CLASS_UNUSED]     = LEXER_STATE_COMPLETE_OPERATOR,
    [CHARACTER_CLASS_ALPHA]      = LEXER_STATE_COMPLETE_OPERATOR,
    [CHARACTER_CLASS_DIGIT]      = LEXER_STATE_NUMBER,
    [CHARACTER_CLASS_HASH]       = LEXER_STATE_COMPLETE_OPERATOR,
    [CHARACTER_CLASS_MINUS]      = LEXER_STATE_OPERATOR,
    [CHARACTER_CLASS_NEWLINE]    = LEXER_STATE_COMPLETE_OPERATOR,
    [CHARACTER_CLASS_NULL]       = LEXER_STATE_COMPLETE_OPERATOR,
    [CHARACTER_CLASS_OPERATOR]   = LEXER_STATE_OPERATOR,
    [CHARACTER_CLASS_PUNCT]      = LEXER_STATE_COMPLETE_OPERATOR,
    [CHARACTER_CLASS_WHITESPACE] = LEXER_STATE_COMPLETE_OPERATOR,
  },
  [LEXER_STATE_NUMBER] = {
    [CHARACTER_CLASS_UNUSED]     = LEXER_STATE_COMPLETE_NUMBER,
    [CHARACTER_CLASS_ALPHA]      = LEXER_STATE_COMPLETE_NUMBER,
    [CHARACTER_CLASS_DIGIT]      = LEXER_STATE_NUMBER,
    [CHARACTER_CLASS_HASH]       = LEXER_STATE_COMPLETE_NUMBER,
    [CHARACTER_CLASS_MINUS]      = LEXER_STATE_COMPLETE_NUMBER,
    [CHARACTER_CLASS_NEWLINE]    = LEXER_STATE_COMPLETE_NUMBER,
    [CHARACTER_CLASS_NULL]       = LEXER_STATE_COMPLETE_NUMBER,
    [CHARACTER_CLASS_OPERATOR]   = LEXER_STATE_COMPLETE_NUMBER,
    [CHARACTER_CLASS_PUNCT]      = LEXER_STATE_COMPLETE_NUMBER,
    [CHARACTER_CLASS_WHITESPACE] = LEXER_STATE_COMPLETE_NUMBER,
  },
  [LEXER_STATE_OPERATOR] = {
    [CHARACTER_CLASS_UNUSED]     = LEXER_STATE_COMPLETE_OPERATOR,
    [CHARACTER_CLASS_ALPHA]      = LEXER_STATE_COMPLETE_OPERATOR,
    [CHARACTER_CLASS_DIGIT]      = LEXER_STATE_COMPLETE_OPERATOR,
    [CHARACTER_CLASS_HASH]       = LEXER_STATE_COMPLETE_OPERATOR,
    [CHARACTER_CLASS_MINUS]      = LEXER_STATE_OPERATOR,
    [CHARACTER_CLASS_NEWLINE]    = LEXER_STATE_COMPLETE_OPERATOR,
    [CHARACTER_CLASS_NULL]       = LEXER_STATE_COMPLETE_OPERATOR,
    [CHARACTER_CLASS_OPERATOR]   = LEXER_STATE_OPERATOR,
    [CHARACTER_CLASS_PUNCT]      = LEXER_STATE_COMPLETE_OPERATOR,
    [CHARACTER_CLASS_WHITESPACE] = LEXER_STATE_COMPLETE_OPERATOR,
  },
  [LEXER_STATE_COMMENT] = {
    [CHARACTER_CLASS_UNUSED]     = LEXER_STATE_COMMENT,
    [CHARACTER_CLASS_ALPHA]      = LEXER_STATE_COMMENT,
    [CHARACTER_CLASS_DIGIT]      = LEXER_STATE_COMMENT,
    [CHARACTER_CLASS_HASH]       = LEXER_STATE_COMMENT,
    [CHARACTER_CLASS_MINUS]      = LEXER_STATE_COMMENT,
    [CHARACTER_CLASS_NEWLINE]    = LEXER_STATE_START,
    [CHARACTER_CLASS_NULL]       = LEXER_STATE_EOF,
    [CHARACTER_CLASS_OPERATOR]   = LEXER_STATE_COMMENT,
    [CHARACTER_CLASS_PUNCT]      = LEXER_STATE_COMMENT,
    [CHARACTER_CLASS_WHITESPACE] = LEXER_STATE_COMMENT,
  },
  [LEXER_STATE_START] = {
    [CHARACTER_CLASS_UNUSED]     = LEXER_STATE_ILLEGAL_CHARACTER,
    [CHARACTER_CLASS_ALPHA]      = LEXER_STATE_IDENTIFIER,
    [CHARACTER_CLASS_DIGIT]      = LEXER_STATE_NUMBER,
    [CHARACTER_CLASS_HASH]       = LEXER_STATE_COMMENT,
    [CHARACTER_CLASS_MINUS]      = LEXER_STATE_MINUS,
    [CHARACTER_CLASS_NEWLINE]    = LEXER_STATE_START,
    [CHARACTER_CLASS_NULL]       = LEXER_STATE_EOF,
    [CHARACTER_CLASS_OPERATOR]   = LEXER_STATE_OPERATOR,
    [CHARACTER_CLASS_PUNCT]      = LEXER_STATE_PUNCT,
    [CHARACTER_CLASS_WHITESPACE] = LEXER_STATE_START,
  },
};

static Token (* const lexer_jump_table[LEXER_STATE_COUNT])(LexerState, char const *, i64) = {
  [LEXER_STATE_IDENTIFIER]          = next_token__loop,
  [LEXER_STATE_MINUS]               = next_token__loop,
  [LEXER_STATE_NUMBER]              = next_token__loop,
  [LEXER_STATE_OPERATOR]            = next_token__loop,
  [LEXER_STATE_COMMENT]             = next_token__loop,
  [LEXER_STATE_START]               = next_token__loop,
  [LEXER_STATE_COMPLETE_IDENTIFIER] = next_token__identifier,
  [LEXER_STATE_COMPLETE_NUMBER]     = next_token__number,
  [LEXER_STATE_COMPLETE_OPERATOR]   = next_token__operator,
  [LEXER_STATE_EOF]                 = next_token__eof,
  [LEXER_STATE_ILLEGAL_CHARACTER]   = next_token__illegal_character,
  [LEXER_STATE_PUNCT]               = next_token__punct,
};
