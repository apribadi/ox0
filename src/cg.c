// code generation
//
// generates bytecode in a single pass

typedef struct {
  bc_buf_t buf; // bytecode buffer
  u8 dst; // destination register for current expression
  u8 reg; // next register name
} cg_t;

static cg_t cg_make(void) {
  cg_t t;

  t.buf = bc_buf_make();
  t.dst = 0;

  return t;
}
