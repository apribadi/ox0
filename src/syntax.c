typedef enum : u8 {
  EXPRESSION_TAG_ADD,
  EXPRESSION_TAG_SUB,
  EXPRESSION_TAG_CONST,
} ExpressionTag;

struct ExpressionAdd;
struct ExpressionSub;

typedef struct Expression {
  ExpressionTag tag;
  union {
    struct ExpressionAdd * add;
    struct ExpressionSub * sub;
  } as;
} Expression;

typedef struct ExpressionAdd {
  Expression right;
  Expression left;
} ExpressionAdd;

typedef struct ExpressionSub {
  Expression right;
  Expression left;
} ExpressionSub;
