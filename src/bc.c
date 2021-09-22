// bc_op

typedef u8 bc_op_t;

enum {
  BC_OP_RETURN,
  BC_OP_CONSTANT, // rA <- #D
  BC_OP_ADD,      // rA <- rB + rC
  BC_OP_SUB,      // rA <- rB - rC
  BC_OP_MUL,      // rA <- rB * rC
};

// bc
//
// <-- least significant ... most significant --->
//
// |-------|-------|-------|-------|
//    op       A       B       C
//
// |-------|-------|---------------|
//    op       A           D

typedef u32 bc_t;

static inline bc_op_t bc_op(bc_t t) {
  return t;
}

static inline bc_t bc_make_3(bc_op_t op, u8 a, u8 b, u8 c) {
  return
      ((u32) op)
    | ((u32) a << 8)
    | ((u32) b << 16)
    | ((u32) c << 24);
}

static inline bc_t bc_make_2(bc_op_t op, u8 a, u16 d) {
  return
      ((u32) op)
    | ((u32) a << 8)
    | ((u32) d << 16);
}

static inline u8 bc_a(bc_t t) {
  return t >> 8;
}

static inline u8 bc_b(bc_t t) {
  return t >> 16;
}

static inline u8 bc_c(bc_t t) {
  return t >> 24;
}

static inline u16 bc_d(bc_t t) {
  return t >> 16;
}

// RETURN

static inline bc_t bc_return(void) {
  return BC_OP_RETURN;
}

static inline bc_t bc_constant(u8 dst, i16 imm) {
  return bc_make_2(BC_OP_CONSTANT, dst, imm);
}

static inline bc_t bc_add(u8 dst, u8 x, u8 y) {
  return bc_make_3(BC_OP_ADD, dst, x, y);
}

static void bc_show(bc_t t) {
  switch (bc_op(t)) {
    case BC_OP_RETURN:
      printf("RETURN\n");
      break;
    case BC_OP_CONSTANT:
      printf("CONSTANT: r%d <- #%d\n", bc_a(t), bc_d(t));
      break;
    case BC_OP_ADD:
      printf("ADD: r%d <- r%d + r%d\n", bc_a(t), bc_b(t), bc_c(t));
      break;
    case BC_OP_SUB:
      printf("SUB: r%d <- r%d * r%d\n", bc_a(t), bc_b(t), bc_c(t));
      break;
    case BC_OP_MUL:
      printf("MUL: r%d <- r%d * r%d\n", bc_a(t), bc_b(t), bc_c(t));
      break;
    default:
      printf("UNKNOWN\n");
      break;
  }
}

// bc_prog

typedef struct {
  i64 count;
  bc_t code[];
} bc_prog_t;

static void bc_prog_show(bc_prog_t * t) {
  i64 count = t->count;

  for (i64 i = 0; i < count; i ++) {
    bc_show(t->code[i]);
  }
}
 
// bc_buf

typedef struct {
  i64 count;
  i64 capacity;
  bc_t * code;
} bc_buf_t;

static void bc_buf_make(bc_buf_t * t) {
  t->count = 0;
  t->capacity = 0;
  t->code = NULL;
}

static void bc_buf_add(bc_buf_t * t, bc_t bc) {
  if (t->count + 1 > t->capacity) {
    i64 old_capacity = t->capacity;
    i64 new_capacity = old_capacity < 8 ? 8 : 2 * old_capacity;

    bc_t * old_code = t->code;
    bc_t * new_code = allocate(new_capacity * sizeof(bc_t));

    if (old_code) {
      memcpy(new_code, old_code, old_capacity * sizeof(bc_t));
    }

    t->capacity = new_capacity;
    t->code = new_code;
  }

  t->code[t->count ++] = bc;
}

static bc_prog_t * bc_buf_prog(bc_buf_t * t) {
  i64 count = t->count;

  bc_prog_t * prog = allocate(sizeof(bc_prog_t) + sizeof(bc_t) * count);

  prog->count = count;

  for (i64 i = 0; i < count; i ++) {
    prog->code[i] = t->code[i];
  }

  return prog;
}
