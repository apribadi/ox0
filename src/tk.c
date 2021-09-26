// token

enum {
  // punctuation

  TK_ASSIGNMENT,
  TK_COMMA,
  TK_DOT,
  TK_COLON,
  TK_SEMICOLON,
  TK_LBRACE,
  TK_RBRACE,
  TK_LBRACKET,
  TK_RBRACKET,
  TK_LPAREN,
  TK_RPAREN,

  // operator

  TK_ADD,
  TK_SUB,
  TK_MUL,
  TK_DIV,
  TK_NEG,
  TK_EQ,
  TK_NE,
  TK_GE,
  TK_GT,
  TK_LE,
  TK_LT,

  // keyword

  TK_AND,
  TK_CASE,
  TK_DO,
  TK_ELIF,
  TK_ELSE,
  TK_END,
  TK_FOR,
  TK_FUN,
  TK_IF,
  TK_LET,
  TK_OR,
  TK_THEN,
  TK_WHILE,

  TK_ID,
  TK_NUMBER,

  TK_EOF,
  TK_ERROR,
};

typedef u8 tk_tag_t;

typedef struct {
  tk_tag_t tag;
  char * start;
  char * stop;
} tk_t;

static inline tk_t tk_make(tk_tag_t tag, char * start, char * stop) {
  return (tk_t) { .tag = tag, .start = start, .stop = stop };
}

static inline i64 tk_len(tk_t t) {
  return t.stop - t.start;
}

static char const * tk_name(tk_t t) {
  switch (t.tag) {
    case TK_ASSIGNMENT: return "ASSIGNMENT";
    case TK_COMMA: return "COMMA";
    case TK_DOT: return "DOT";
    case TK_COLON: return "COLON";
    case TK_SEMICOLON: return "SEMICOLON";
    case TK_LBRACE: return "LBRACE";
    case TK_RBRACE: return "RBRACE";
    case TK_LBRACKET: return "LBRACKET";
    case TK_RBRACKET: return "RBRACKET";
    case TK_LPAREN: return "LPAREN";
    case TK_RPAREN: return "RPAREN";
    case TK_ADD: return "ADD";
    case TK_SUB: return "SUB";
    case TK_MUL: return "MUL";
    case TK_DIV: return "DIV";
    case TK_NEG: return "NEG";
    case TK_EQ: return "EQ";
    case TK_NE: return "NE";
    case TK_GE: return "GE";
    case TK_GT: return "GT";
    case TK_LE: return "LE";
    case TK_LT: return "LT";
    case TK_AND: return "AND";
    case TK_CASE: return "CASE";
    case TK_DO: return "DO";
    case TK_ELIF: return "ELIF";
    case TK_ELSE: return "ELSE";
    case TK_END: return "END";
    case TK_FOR: return "FOR";
    case TK_FUN: return "FUN";
    case TK_IF: return "IF";
    case TK_LET: return "LET";
    case TK_OR: return "OR";
    case TK_THEN: return "THEN";
    case TK_WHILE: return "WHILE";
    case TK_ID: return "ID";
    case TK_NUMBER: return "NUMBER";
    case TK_EOF: return "EOF";
    case TK_ERROR: return "ERROR";
  }

  return "UNKNOWN";
}

static void tk_show(tk_t t) {
  printf("%s: %.*s\n", tk_name(t), (int) tk_len(t), t.start);
}
