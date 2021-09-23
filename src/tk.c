// token

enum {
  // punctuation

  TK_LET_EQUAL,
  TK_COMMA,
  TK_PERIOD,
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
  TK_EQ,
  TK_NQ,
  TK_GE,
  TK_GT,
  TK_LE,
  TK_LT,

  // keyword

  TK_BREAK,
  TK_CASE,
  TK_DO,
  TK_ELIF,
  TK_ELSE,
  TK_END,
  TK_FOR,
  TK_IF,
  TK_LET,
  TK_THEN,
  TK_WHILE,

  TK_ID,
  TK_EOF,
  TK_ERROR,
};

typedef struct {
  u8 tag;
  u8 * start;
  u8 * stop;
} tk_t;

static inline tk_t tk_make(u8 tag, u8 * start, u8 * stop) {
  return (tk_t) { .tag = tag, .start = start, .stop = stop };
}

static inline i64 tk_len(tk_t t) {
  return t.stop - t.start;
}

static char const * tk_name(tk_t t) {
  switch (t.tag) {
    case TK_LET_EQUAL: return "LET_EQUAL";
    case TK_COMMA: return "COMMA";
    case TK_PERIOD: return "PERIOD";
    case TK_COLON: return "COLON";
    case TK_SEMICOLON: return "SEMICOLON";
    case TK_LBRACE: return "LBRACE";
    case TK_RBRACE: return "RBRACE";
    case TK_LBRACKET: return "LBRACKET";
    case TK_RBRACKET: return "RBRACKET";
    case TK_LPAREN: return "LPAREN";
    case TK_RPAREN: return "RPAREN";
    case TK_ID: return "ID";
    case TK_EOF: return "EOF";
    case TK_ERROR: return "ERROR";
  }

  return "UNKNOWN";
}

static void tk_show(tk_t t) {
  printf("%s: %.*s\n", tk_name(t), (int) tk_len(t), t.start);
}
