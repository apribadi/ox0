typedef enum : u8 {
  EXPR_OP_NEG,
} ExprUnaryOp;

typedef enum : u8 {
  EXPR_OP_ADD,
  EXPR_OP_DIV,
  EXPR_OP_MUL,
  EXPR_OP_SUB,
} ExprBinaryOp;

typedef enum : u8 {
  EXPR_TAG_ERROR,
  EXPR_TAG_UNARY_OP,
  EXPR_TAG_BINARY_OP,
  EXPR_TAG_LITERAL,
} ExprTag;

struct ExprUnary;
struct ExprBinary;

typedef struct Expr {
  ExprTag tag;
  union {
    struct ExprUnary * unary;
    struct ExprBinary * binary;
    Symbol literal;
  } as;
} Expr;

typedef struct ExprUnary {
  ExprUnaryOp op;
  Expr inner;
} ExprUnary;

typedef struct ExprBinary {
  ExprBinaryOp op;
  Expr left;
  Expr right;
} ExprBinary;

Expr syntax_make_error(void) {
  Expr exp;
  exp.tag = EXPR_TAG_ERROR;
  return exp;
}

Expr syntax_make_unary(
      Arena * arena,
      ExprUnaryOp op,
      Expr inner
    ) {
  ExprUnary * unary = arena_alloc(arena, sizeof(ExprUnary));
  unary->op = op;
  unary->inner = inner;
  Expr exp;
  exp.tag = EXPR_TAG_UNARY_OP;
  exp.as.unary = unary;
  return exp;
}

Expr syntax_make_binary(
      Arena * arena,
      ExprBinaryOp op,
      Expr left,
      Expr right
    ) {
  ExprBinary * binary = arena_alloc(arena, sizeof(ExprBinary));
  binary->op = op;
  binary->left = left;
  binary->right = right;
  Expr exp;
  exp.tag = EXPR_TAG_BINARY_OP;
  exp.as.binary = binary;
  return exp;
}

Expr syntax_make_literal(Symbol symbol) {
  Expr exp;
  exp.tag = EXPR_TAG_LITERAL;
  exp.as.literal = symbol;
  return exp;
}
