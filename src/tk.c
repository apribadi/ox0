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

typedef struct {
  i64 len;
  tk_t data[];
} tk_vec_t;

static tk_vec_t * tk_vec_make(i64 len) {
  tk_vec_t * t = mm_alloc(sizeof(tk_vec_t) + sizeof(tk_t) * len);
  t->len = len;
  return t;
}

typedef struct {
  i64 len;
  i64 cap;
  tk_t * data;
} tk_buf_t;

static tk_buf_t tk_buf_make(void) {
  return (tk_buf_t) {
      .len = 0,
      .cap = 0,
      .data = NULL,
  };
}

static void tk_buf_add(tk_buf_t * t, tk_t elt) {
  if (t->len >= t->cap) {
    i64 old_cap = t->cap;
    i64 new_cap = old_cap < 8 ? 8 : 2 * old_cap;

    tk_t * old_data = t->data;
    tk_t * new_data = mm_alloc(new_cap * sizeof(tk_t));

    if (old_data) memcpy(new_data, old_data, old_cap * sizeof(tk_t));

    t->cap = new_cap;
    t->data = new_data;
  }

  t->data[t->len ++] = elt;
}

static tk_vec_t * tk_buf_vec(tk_buf_t * t) {
  i64 n = t->len;

  tk_vec_t * r = tk_vec_make(n);

  for (i64 i = 0; i < n; i ++) {
    r->data[i] = t->data[i];
  }

  return r;
}
