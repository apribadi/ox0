// token

enum {

  // punctuation

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

  // operators

  TK_EQUAL,
  TK_GE,
  TK_GT,
  TK_LE,
  TK_LT,
  TK_MINUS,
  TK_PLUS,
  TK_SLASH,
  TK_STAR,

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

  TK_COMMENT,
  TK_EOF,
  TK_ERROR,
  TK_ID,
  TK_SPACE,
};

typedef struct {
  u8 tag;
  u8 * start;
  u8 * stop;
} tk_t;

tk_t tk_make(u8 tag, u8 * start, u8 * stop) {
  return (tk_t) { .tag = tag, .start = start, .stop = stop };
}
