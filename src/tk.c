// token

enum {
  // grouping

  TK_LBRACE,
  TK_RBRACE,
  TK_LBRACKET,
  TK_RBRACKET,
  TK_LPAREN,
  TK_RPAREN,

  // punctuation

  TK_COLON,
  TK_COMMA,
  TK_DOT,
  TK_EQUAL,
  TK_GE,
  TK_GT,
  TK_LE,
  TK_LT,
  TK_MINUS,
  TK_PLUS,
  TK_SEMICOLON,
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
  i16 len;
  i32 ofs;
} tk_t;
