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
    SyntaxLiteral literal;
  } as;
} CodegenRegOrLiteral;

/*
define i64 @foo(i64 %0, i64 %1) {
2:
  %3 = add i64 %0, %1
  %4 = add i64 %3, 100
  ret i64 %4
}
*/

CodegenRegOrLiteral codegen_reg(i64 label) {
  CodegenRegOrLiteral result;
  result.tag = CODEGEN_REG;
  result.as.reg = label;
  return result;
}

CodegenRegOrLiteral codegen_literal(SyntaxLiteral literal) {
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
      printf("%.*s", (int) (arg.as.literal.stop - arg.as.literal.start), arg.as.literal.start);
      break;
  }
}

CodegenRegOrLiteral codegen_emit_expression(Codegen * cg, SyntaxExpression exp) {
  switch (exp.tag) {
    case SYNTAX_EXPRESSION_TAG_UNARY_OPERATOR:
      {
        CodegenRegOrLiteral a = codegen_emit_expression(cg, exp.as.unary->inner);
        i64 label = cg->label ++;
        printf("  %%%d = ", (int) label);
        switch (exp.as.unary->operator) {
          case SYNTAX_OPERATOR_NEG:
            printf("sub i64 0, ");
            codegen_emit_reg_or_literal(a);
            printf("\n");
            break;
        }
        return codegen_reg(label);
      }
    case SYNTAX_EXPRESSION_TAG_BINARY_OPERATOR:
      {
        CodegenRegOrLiteral a = codegen_emit_expression(cg, exp.as.binary->left);
        CodegenRegOrLiteral b = codegen_emit_expression(cg, exp.as.binary->right);
        i64 label = cg->label ++;
        printf("  %%%d = ", (int) label);
        switch (exp.as.binary->operator) {
          case SYNTAX_OPERATOR_ADD:
            printf("add i64 ");
            break;
          case SYNTAX_OPERATOR_SUB:
            printf("sub i64 ");
            break;
          case SYNTAX_OPERATOR_MUL:
            printf("mul i64 ");
            break;
          case SYNTAX_OPERATOR_DIV:
            printf("sdiv i64 ");
            break;
        }
        codegen_emit_reg_or_literal(a);
        printf(", ");
        codegen_emit_reg_or_literal(b);
        printf("\n");
        return codegen_reg(label);
      }
    case SYNTAX_EXPRESSION_TAG_LITERAL:
      return codegen_literal(* exp.as.literal);
  }

  return codegen_reg(-1); // ???
}

void codegen_emit_function(SyntaxExpression exp) {
  Codegen cg = { 0 };

  printf("define i64 @foo() {\n");
  printf("%d:\n", (int) (cg.label ++));
  CodegenRegOrLiteral result = codegen_emit_expression(&cg, exp);
  printf("  ret i64 ");
  codegen_emit_reg_or_literal(result);
  printf("\n");
  printf("}\n");
}
