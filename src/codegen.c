typedef struct {
  i64 next_label;
} Codegen;

typedef enum : u8 {
  REGISTER_OR_CONSTANT_TAG_REGISTER,
  REGISTER_OR_CONSTANT_TAG_CONSTANT
} RegisterOrConstantTag;

typedef struct {
  RegisterOrConstantTag tag;
  union { i64 reg; Symbol constant; } as;
} RegisterOrConstant;

/*
define i64 @foo(i64 %0, i64 %1) {
2:
  %3 = add i64 %0, %1
  %4 = add i64 %3, 100
  ret i64 %4
}
*/

void codegen_emit_function_prologue(Codegen * cg) {
  (void) cg;

  printf("define i64 @foo() {\n");
}

void codegen_emit_function_epilogue(Codegen * cg) {
  (void) cg;

  printf("}\n");
}

i64 codegen_emit_constant(Codegen * cg, Symbol literal) {
  i64 label = cg->next_label ++;

  printf("%%%d = add i64 0, %.*s\n", (int) label, (int) literal.length, literal.data);

  return label;
}
