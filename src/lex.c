// lexing
//
// Lex lex_make(char const * start)
//
// Token lex_next(Lex * t)

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
  TOKEN_FUN,
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
    case TOKEN_FUN: return "FUN";
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
} Lex;

static inline Lex lex_make(char const * start) {
  // Input string MUST be terminated by a '\0' character.

  return (Lex) { .pos = start };
}

typedef enum : u8 {
  LEX_CLASS_UNUSED = 0,
  LEX_CLASS_ALPHA,
  LEX_CLASS_DIGIT,
  LEX_CLASS_HASH,
  LEX_CLASS_MINUS,
  LEX_CLASS_NEWLINE,
  LEX_CLASS_NULL,
  LEX_CLASS_OPERATOR,
  LEX_CLASS_PUNCT,
  LEX_CLASS_WHITESPACE,
} LexCharacterClass;

#define LEX_CLASS_COUNT 10

typedef enum : u8 {
  LEX_STATE_IDENTIFIER,
  LEX_STATE_MINUS,
  LEX_STATE_NUMBER,
  LEX_STATE_OPERATOR,
  LEX_STATE_COMMENT,
  LEX_STATE_START,
  LEX_STATE_COMPLETE_IDENTIFIER,
  LEX_STATE_COMPLETE_NUMBER,
  LEX_STATE_COMPLETE_OPERATOR,
  LEX_STATE_EOF,
  LEX_STATE_ILLEGAL_CHARACTER,
  LEX_STATE_PUNCT,
} LexState;

#define LEX_STATE_SPANNED_COUNT 4
#define LEX_STATE_NONTERMINAL_COUNT 6
#define LEX_STATE_COUNT 12

static LexCharacterClass const lex_character_class_table[256];

static LexState const lex_transition_table[LEX_STATE_NONTERMINAL_COUNT][LEX_CLASS_COUNT];

static Token (* const lex_jump_table[LEX_STATE_COUNT])(LexState, char const *, i64);

static Token lex_next__loop(LexState s, char const * p, i64 n) {
  n = n + (s < LEX_STATE_SPANNED_COUNT);
  s = lex_transition_table[s][lex_character_class_table[(u8) * p]];
  p = p + 1;

  return lex_jump_table[s](s, p, n);
}

static inline Token lex_next(Lex * t) {
  Token r = lex_next__loop(LEX_STATE_START, t->pos, 0);
  t->pos = r.stop;
  return r;
}

static Token lex_next__eof(__unused LexState s, char const * p, __unused i64 n) {
  char const * start = p - 1;
  char const * stop = p - 1;

  return token_make(TOKEN_EOF, start, stop);
}

static Token lex_next__illegal_character(__unused LexState s, char const * p, __unused i64 n) {
  char const * start = p - 1;
  char const * stop = p;

  return token_make(TOKEN_ILLEGAL_CHARACTER, start, stop);
}

static Token lex_next__identifier(__unused LexState s, char const * p, i64 n) {
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
      if (!bcmp(start, "fun", 3)) return token_make(TOKEN_FUN, start, stop);
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
  }

  return token_make(TOKEN_IDENTIFIER, start, stop);
}

static Token lex_next__number(__unused LexState s, char const * p, i64 n) {
  char const * start = p - 1 - n;
  char const * stop = p - 1;

  return token_make(TOKEN_NUMBER, start, stop);
}

static Token lex_next__operator(__unused LexState s, char const * p, i64 n) {
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

static Token lex_next__punct(__unused LexState s, char const * p, __unused i64 n) {
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

static LexCharacterClass const lex_character_class_table[256] = {
  ['\0'] = LEX_CLASS_NULL,
  ['\t'] = LEX_CLASS_WHITESPACE,
  ['\n'] = LEX_CLASS_NEWLINE,
  [' '] = LEX_CLASS_WHITESPACE,
  ['!'] = LEX_CLASS_OPERATOR,
  ['#'] = LEX_CLASS_HASH,
  ['$'] = LEX_CLASS_OPERATOR,
  ['%'] = LEX_CLASS_OPERATOR,
  ['&'] = LEX_CLASS_OPERATOR,
  ['('] = LEX_CLASS_PUNCT,
  [')'] = LEX_CLASS_PUNCT,
  ['*'] = LEX_CLASS_OPERATOR,
  ['+'] = LEX_CLASS_OPERATOR,
  [','] = LEX_CLASS_PUNCT,
  ['-'] = LEX_CLASS_MINUS,
  ['.'] = LEX_CLASS_PUNCT,
  ['/'] = LEX_CLASS_OPERATOR,
  ['0'] = LEX_CLASS_DIGIT,
  ['1'] = LEX_CLASS_DIGIT,
  ['2'] = LEX_CLASS_DIGIT,
  ['3'] = LEX_CLASS_DIGIT,
  ['4'] = LEX_CLASS_DIGIT,
  ['5'] = LEX_CLASS_DIGIT,
  ['6'] = LEX_CLASS_DIGIT,
  ['7'] = LEX_CLASS_DIGIT,
  ['8'] = LEX_CLASS_DIGIT,
  ['9'] = LEX_CLASS_DIGIT,
  [':'] = LEX_CLASS_PUNCT,
  [';'] = LEX_CLASS_PUNCT,
  ['<'] = LEX_CLASS_OPERATOR,
  ['='] = LEX_CLASS_OPERATOR,
  ['>'] = LEX_CLASS_OPERATOR,
  ['?'] = LEX_CLASS_OPERATOR,
  ['@'] = LEX_CLASS_OPERATOR,
  ['A'] = LEX_CLASS_ALPHA,
  ['B'] = LEX_CLASS_ALPHA,
  ['C'] = LEX_CLASS_ALPHA,
  ['D'] = LEX_CLASS_ALPHA,
  ['E'] = LEX_CLASS_ALPHA,
  ['F'] = LEX_CLASS_ALPHA,
  ['G'] = LEX_CLASS_ALPHA,
  ['H'] = LEX_CLASS_ALPHA,
  ['I'] = LEX_CLASS_ALPHA,
  ['J'] = LEX_CLASS_ALPHA,
  ['K'] = LEX_CLASS_ALPHA,
  ['L'] = LEX_CLASS_ALPHA,
  ['M'] = LEX_CLASS_ALPHA,
  ['N'] = LEX_CLASS_ALPHA,
  ['O'] = LEX_CLASS_ALPHA,
  ['P'] = LEX_CLASS_ALPHA,
  ['Q'] = LEX_CLASS_ALPHA,
  ['R'] = LEX_CLASS_ALPHA,
  ['S'] = LEX_CLASS_ALPHA,
  ['T'] = LEX_CLASS_ALPHA,
  ['U'] = LEX_CLASS_ALPHA,
  ['V'] = LEX_CLASS_ALPHA,
  ['W'] = LEX_CLASS_ALPHA,
  ['X'] = LEX_CLASS_ALPHA,
  ['Y'] = LEX_CLASS_ALPHA,
  ['Z'] = LEX_CLASS_ALPHA,
  ['['] = LEX_CLASS_PUNCT,
  [']'] = LEX_CLASS_PUNCT,
  ['^'] = LEX_CLASS_OPERATOR,
  ['_'] = LEX_CLASS_ALPHA,
  ['a'] = LEX_CLASS_ALPHA,
  ['b'] = LEX_CLASS_ALPHA,
  ['c'] = LEX_CLASS_ALPHA,
  ['d'] = LEX_CLASS_ALPHA,
  ['e'] = LEX_CLASS_ALPHA,
  ['f'] = LEX_CLASS_ALPHA,
  ['g'] = LEX_CLASS_ALPHA,
  ['h'] = LEX_CLASS_ALPHA,
  ['i'] = LEX_CLASS_ALPHA,
  ['j'] = LEX_CLASS_ALPHA,
  ['k'] = LEX_CLASS_ALPHA,
  ['l'] = LEX_CLASS_ALPHA,
  ['m'] = LEX_CLASS_ALPHA,
  ['n'] = LEX_CLASS_ALPHA,
  ['o'] = LEX_CLASS_ALPHA,
  ['p'] = LEX_CLASS_ALPHA,
  ['q'] = LEX_CLASS_ALPHA,
  ['r'] = LEX_CLASS_ALPHA,
  ['s'] = LEX_CLASS_ALPHA,
  ['t'] = LEX_CLASS_ALPHA,
  ['u'] = LEX_CLASS_ALPHA,
  ['v'] = LEX_CLASS_ALPHA,
  ['w'] = LEX_CLASS_ALPHA,
  ['x'] = LEX_CLASS_ALPHA,
  ['y'] = LEX_CLASS_ALPHA,
  ['z'] = LEX_CLASS_ALPHA,
  ['{'] = LEX_CLASS_PUNCT,
  ['|'] = LEX_CLASS_OPERATOR,
  ['}'] = LEX_CLASS_PUNCT,
  ['~'] = LEX_CLASS_OPERATOR,
};

static LexState const lex_transition_table[LEX_STATE_NONTERMINAL_COUNT][LEX_CLASS_COUNT] = {
  [LEX_STATE_IDENTIFIER] = {
    [LEX_CLASS_UNUSED]     = LEX_STATE_COMPLETE_IDENTIFIER,
    [LEX_CLASS_ALPHA]      = LEX_STATE_IDENTIFIER,
    [LEX_CLASS_DIGIT]      = LEX_STATE_IDENTIFIER,
    [LEX_CLASS_HASH]       = LEX_STATE_COMPLETE_IDENTIFIER,
    [LEX_CLASS_MINUS]      = LEX_STATE_COMPLETE_IDENTIFIER,
    [LEX_CLASS_NEWLINE]    = LEX_STATE_COMPLETE_IDENTIFIER,
    [LEX_CLASS_NULL]       = LEX_STATE_COMPLETE_IDENTIFIER,
    [LEX_CLASS_OPERATOR]   = LEX_STATE_COMPLETE_IDENTIFIER,
    [LEX_CLASS_PUNCT]      = LEX_STATE_COMPLETE_IDENTIFIER,
    [LEX_CLASS_WHITESPACE] = LEX_STATE_COMPLETE_IDENTIFIER,
  },
  [LEX_STATE_MINUS] = {
    [LEX_CLASS_UNUSED]     = LEX_STATE_COMPLETE_OPERATOR,
    [LEX_CLASS_ALPHA]      = LEX_STATE_COMPLETE_OPERATOR,
    [LEX_CLASS_DIGIT]      = LEX_STATE_NUMBER,
    [LEX_CLASS_HASH]       = LEX_STATE_COMPLETE_OPERATOR,
    [LEX_CLASS_MINUS]      = LEX_STATE_OPERATOR,
    [LEX_CLASS_NEWLINE]    = LEX_STATE_COMPLETE_OPERATOR,
    [LEX_CLASS_NULL]       = LEX_STATE_COMPLETE_OPERATOR,
    [LEX_CLASS_OPERATOR]   = LEX_STATE_OPERATOR,
    [LEX_CLASS_PUNCT]      = LEX_STATE_COMPLETE_OPERATOR,
    [LEX_CLASS_WHITESPACE] = LEX_STATE_COMPLETE_OPERATOR,
  },
  [LEX_STATE_NUMBER] = {
    [LEX_CLASS_UNUSED]     = LEX_STATE_COMPLETE_NUMBER,
    [LEX_CLASS_ALPHA]      = LEX_STATE_COMPLETE_NUMBER,
    [LEX_CLASS_DIGIT]      = LEX_STATE_NUMBER,
    [LEX_CLASS_HASH]       = LEX_STATE_COMPLETE_NUMBER,
    [LEX_CLASS_MINUS]      = LEX_STATE_COMPLETE_NUMBER,
    [LEX_CLASS_NEWLINE]    = LEX_STATE_COMPLETE_NUMBER,
    [LEX_CLASS_NULL]       = LEX_STATE_COMPLETE_NUMBER,
    [LEX_CLASS_OPERATOR]   = LEX_STATE_COMPLETE_NUMBER,
    [LEX_CLASS_PUNCT]      = LEX_STATE_COMPLETE_NUMBER,
    [LEX_CLASS_WHITESPACE] = LEX_STATE_COMPLETE_NUMBER,
  },
  [LEX_STATE_OPERATOR] = {
    [LEX_CLASS_UNUSED]     = LEX_STATE_COMPLETE_OPERATOR,
    [LEX_CLASS_ALPHA]      = LEX_STATE_COMPLETE_OPERATOR,
    [LEX_CLASS_DIGIT]      = LEX_STATE_COMPLETE_OPERATOR,
    [LEX_CLASS_HASH]       = LEX_STATE_COMPLETE_OPERATOR,
    [LEX_CLASS_MINUS]      = LEX_STATE_OPERATOR,
    [LEX_CLASS_NEWLINE]    = LEX_STATE_COMPLETE_OPERATOR,
    [LEX_CLASS_NULL]       = LEX_STATE_COMPLETE_OPERATOR,
    [LEX_CLASS_OPERATOR]   = LEX_STATE_OPERATOR,
    [LEX_CLASS_PUNCT]      = LEX_STATE_COMPLETE_OPERATOR,
    [LEX_CLASS_WHITESPACE] = LEX_STATE_COMPLETE_OPERATOR,
  },
  [LEX_STATE_COMMENT] = {
    [LEX_CLASS_UNUSED]     = LEX_STATE_COMMENT,
    [LEX_CLASS_ALPHA]      = LEX_STATE_COMMENT,
    [LEX_CLASS_DIGIT]      = LEX_STATE_COMMENT,
    [LEX_CLASS_HASH]       = LEX_STATE_COMMENT,
    [LEX_CLASS_MINUS]      = LEX_STATE_COMMENT,
    [LEX_CLASS_NEWLINE]    = LEX_STATE_START,
    [LEX_CLASS_NULL]       = LEX_STATE_EOF,
    [LEX_CLASS_OPERATOR]   = LEX_STATE_COMMENT,
    [LEX_CLASS_PUNCT]      = LEX_STATE_COMMENT,
    [LEX_CLASS_WHITESPACE] = LEX_STATE_COMMENT,
  },
  [LEX_STATE_START] = {
    [LEX_CLASS_UNUSED]     = LEX_STATE_ILLEGAL_CHARACTER,
    [LEX_CLASS_ALPHA]      = LEX_STATE_IDENTIFIER,
    [LEX_CLASS_DIGIT]      = LEX_STATE_NUMBER,
    [LEX_CLASS_HASH]       = LEX_STATE_COMMENT,
    [LEX_CLASS_MINUS]      = LEX_STATE_MINUS,
    [LEX_CLASS_NEWLINE]    = LEX_STATE_START,
    [LEX_CLASS_NULL]       = LEX_STATE_EOF,
    [LEX_CLASS_OPERATOR]   = LEX_STATE_OPERATOR,
    [LEX_CLASS_PUNCT]      = LEX_STATE_PUNCT,
    [LEX_CLASS_WHITESPACE] = LEX_STATE_START,
  },
};

static Token (* const lex_jump_table[LEX_STATE_COUNT])(LexState, char const *, i64) = {
  [LEX_STATE_IDENTIFIER]          = lex_next__loop,
  [LEX_STATE_MINUS]               = lex_next__loop,
  [LEX_STATE_NUMBER]              = lex_next__loop,
  [LEX_STATE_OPERATOR]            = lex_next__loop,
  [LEX_STATE_COMMENT]             = lex_next__loop,
  [LEX_STATE_START]               = lex_next__loop,
  [LEX_STATE_COMPLETE_IDENTIFIER] = lex_next__identifier,
  [LEX_STATE_COMPLETE_NUMBER]     = lex_next__number,
  [LEX_STATE_COMPLETE_OPERATOR]   = lex_next__operator,
  [LEX_STATE_EOF]                 = lex_next__eof,
  [LEX_STATE_ILLEGAL_CHARACTER]   = lex_next__illegal_character,
  [LEX_STATE_PUNCT]               = lex_next__punct,
};
