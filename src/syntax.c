typedef enum : u8 {
  SYNTAX_OPERATOR_NEG,
} SyntaxUnaryOperator;

typedef enum : u8 {
  SYNTAX_OPERATOR_ADD,
  SYNTAX_OPERATOR_DIV,
  SYNTAX_OPERATOR_MUL,
  SYNTAX_OPERATOR_SUB,
} SyntaxBinaryOperator;

typedef enum : u8 {
  SYNTAX_EXPRESSION_TAG_ERROR,
  SYNTAX_EXPRESSION_TAG_UNARY_OPERATOR,
  SYNTAX_EXPRESSION_TAG_BINARY_OPERATOR,
  SYNTAX_EXPRESSION_TAG_LITERAL,
} SyntaxExpressionTag;

struct SyntaxUnary;
struct SyntaxBinary;

typedef struct SyntaxExpression {
  SyntaxExpressionTag tag;
  union {
    struct SyntaxUnary * unary;
    struct SyntaxBinary * binary;
    Symbol literal;
  } as;
} SyntaxExpression;

typedef struct SyntaxUnary {
  SyntaxUnaryOperator operator;
  SyntaxExpression inner;
} SyntaxUnary;

typedef struct SyntaxBinary {
  SyntaxBinaryOperator operator;
  SyntaxExpression left;
  SyntaxExpression right;
} SyntaxBinary;

SyntaxExpression syntax_make_error(void) {
  SyntaxExpression exp;
  exp.tag = SYNTAX_EXPRESSION_TAG_ERROR;
  return exp;
}

SyntaxExpression syntax_make_unary(
      Arena * arena,
      SyntaxUnaryOperator operator,
      SyntaxExpression inner
    ) {
  SyntaxUnary * unary = arena_alloc(arena, sizeof(SyntaxUnary));
  unary->operator = operator;
  unary->inner = inner;
  SyntaxExpression exp;
  exp.tag = SYNTAX_EXPRESSION_TAG_UNARY_OPERATOR;
  exp.as.unary = unary;
  return exp;
}

SyntaxExpression syntax_make_binary(
      Arena * arena,
      SyntaxBinaryOperator operator,
      SyntaxExpression left,
      SyntaxExpression right
    ) {
  SyntaxBinary * binary = arena_alloc(arena, sizeof(SyntaxBinary));
  binary->operator = operator;
  binary->left = left;
  binary->right = right;
  SyntaxExpression exp;
  exp.tag = SYNTAX_EXPRESSION_TAG_BINARY_OPERATOR;
  exp.as.binary = binary;
  return exp;
}

SyntaxExpression syntax_make_literal(Symbol symbol) {
  SyntaxExpression exp;
  exp.tag = SYNTAX_EXPRESSION_TAG_LITERAL;
  exp.as.literal = symbol;
  return exp;
}
