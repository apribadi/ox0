typedef enum : u8 {
  EXPR_OP_NEG,
} ExprPrefixOp;

typedef enum : u8 {
  EXPR_OP_ADD,
  EXPR_OP_DIV,
  EXPR_OP_MUL,
  EXPR_OP_SUB,
} ExprInfixOp;

typedef enum : u8 {
  EXPR_TAG_UNARY_OP,
  EXPR_TAG_BINARY_OP,
  EXPR_TAG_LITERAL,
} ExprTag;

struct ExprPrefix;
struct ExprInfix;

typedef struct Expr {
  ExprTag tag;
  union {
    struct ExprPrefix * prefix;
    struct ExprInfix * infix;
    Symbol literal;
  } as;
} Expr;

typedef struct ExprPrefix {
  ExprPrefixOp op;
  Expr inner;
} ExprPrefix;

typedef struct ExprInfix {
  ExprInfixOp op;
  Expr left;
  Expr right;
} ExprInfix;

Expr make_expr_prefix(Arena * arena, ExprPrefixOp op, Expr inner) {
  ExprPrefix * prefix = arena_alloc(arena, sizeof(ExprPrefix));
  prefix->op = op;
  prefix->inner = inner;
  Expr exp;
  exp.tag = EXPR_TAG_UNARY_OP;
  exp.as.prefix = prefix;
  return exp;
}

Expr make_expr_infix(Arena * arena, ExprInfixOp op, Expr left, Expr right) {
  ExprInfix * infix = arena_alloc(arena, sizeof(ExprInfix));
  infix->op = op;
  infix->left = left;
  infix->right = right;
  Expr exp;
  exp.tag = EXPR_TAG_BINARY_OP;
  exp.as.infix = infix;
  return exp;
}

Expr make_expr_literal(Symbol symbol) {
  Expr exp;
  exp.tag = EXPR_TAG_LITERAL;
  exp.as.literal = symbol;
  return exp;
}
