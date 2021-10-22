typedef struct {
  i64 label;
} Codegen;

typedef enum : u8 { 
  CODEGEN_REG,
  CODEGEN_LITERAL,
} CodegenRegOrLiteralTag;

typedef struct {
  CodegenRegOrLiteralTag tag;
  union {
    i64 reg;
    Symbol literal;
  } as;
} CodegenRegOrLiteral;

CodegenRegOrLiteral codegen_reg(i64 label) {
  CodegenRegOrLiteral result;
  result.tag = CODEGEN_REG;
  result.as.reg = label;
  return result;
}

CodegenRegOrLiteral codegen_literal(Symbol literal) {
  CodegenRegOrLiteral result;
  result.tag = CODEGEN_LITERAL;
  result.as.literal = literal;
  return result;
}

void codegen_emit_reg_or_literal(CodegenRegOrLiteral arg) {
  switch (arg.tag) {
    case CODEGEN_REG:
      printf("%%%d", (int) arg.as.reg);
      break;
    case CODEGEN_LITERAL: 
      print_symbol(arg.as.literal);
      break;
  }
}

CodegenRegOrLiteral codegen_emit_expression(Codegen * cg, Expr exp) {
  switch (exp.tag) {
    case EXPR_TAG_UNARY_OP:
      {
        CodegenRegOrLiteral a = codegen_emit_expression(cg, exp.as.unary->inner);
        i64 label = cg->label ++;
        printf("  %%%d = ", (int) label);
        switch (exp.as.unary->op) {
          case EXPR_OP_NEG:
            printf("sub i64 0, ");
            codegen_emit_reg_or_literal(a);
            printf("\n");
            break;
        }
        return codegen_reg(label);
      }
    case EXPR_TAG_BINARY_OP:
      {
        CodegenRegOrLiteral a = codegen_emit_expression(cg, exp.as.binary->left);
        CodegenRegOrLiteral b = codegen_emit_expression(cg, exp.as.binary->right);
        i64 label = cg->label ++;
        printf("  %%%d = ", (int) label);
        switch (exp.as.binary->op) {
          case EXPR_OP_ADD:
            printf("add i64 ");
            break;
          case EXPR_OP_SUB:
            printf("sub i64 ");
            break;
          case EXPR_OP_MUL:
            printf("mul i64 ");
            break;
          case EXPR_OP_DIV:
            printf("sdiv i64 ");
            break;
        }
        codegen_emit_reg_or_literal(a);
        printf(", ");
        codegen_emit_reg_or_literal(b);
        printf("\n");
        return codegen_reg(label);
      }
    case EXPR_TAG_LITERAL:
      return codegen_literal(exp.as.literal);
  }

  return codegen_reg(-1); // ???
}

void codegen_emit_function(Expr exp) {
  Codegen cg = { 0 };

  printf("define i64 @foo() {\n");
  printf("%d:\n", (int) (cg.label ++));
  CodegenRegOrLiteral result = codegen_emit_expression(&cg, exp);
  printf("  ret i64 ");
  codegen_emit_reg_or_literal(result);
  printf("\n");
  printf("}\n");
}
