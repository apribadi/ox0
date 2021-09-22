// bytecode

// a bytecode instruction is 32 bits
//
// least significant <---> most significant
//
// |-------|-------|-------|-------|
//    tag      A       B       C
//
// |-------|-------|---------------|
//    tag      A           D

typedef u32 bc_t;

enum {
  BC_EXIT,
  BC_IMM, // rA <- #D
  BC_MOV, // rA <- rB
  BC_ADD, // rA <- rB + rC
  BC_SUB, // rA <- rB - rC
  BC_MUL, // rA <- rB * rC
  BC_NEG, // rA <- - rB
};

// a bytecode instruction can take one of two layouts

static inline bc_t bc_make_3(u8 tag, u8 a, u8 b, u8 c) {
  return
      ((u32) tag)
    | ((u32) a << 8)
    | ((u32) b << 16)
    | ((u32) c << 24);
}

static inline bc_t bc_make_2(u8 tag, u8 a, u16 d) {
  return
      ((u32) tag)
    | ((u32) a << 8)
    | ((u32) d << 16);
}

static inline u8 bc_tag(bc_t t) {
  return t;
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

// bytecode instruction definitions

static inline bc_t bc_exit(void) {
  return BC_EXIT;
}

static inline bc_t bc_imm(u8 ra, i16 imm) {
  return bc_make_2(BC_IMM, ra, imm);
}

static inline bc_t bc_mov(u8 ra, u8 rb) {
  return bc_make_3(BC_MOV, ra, rb, 0);
}

static inline bc_t bc_add(u8 ra, u8 rb, u8 rc) {
  return bc_make_3(BC_ADD, ra, rb, rc);
}

static inline bc_t bc_sub(u8 ra, u8 rb, u8 rc) {
  return bc_make_3(BC_SUB, ra, rb, rc);
}

static inline bc_t bc_mul(u8 ra, u8 rb, u8 rc) {
  return bc_make_3(BC_MUL, ra, rb, rc);
}

static inline bc_t bc_neg(u8 ra, u8 rb) {
  return bc_make_3(BC_NEG, ra, rb, 0);
}

// display a bytecode instruction for debugging

static void bc_show(bc_t t) {
  switch (bc_tag(t)) {
    case BC_EXIT:
      printf("EXIT\n");
      break;
    case BC_IMM:
      printf("IMM: r%d <- #%d\n", bc_a(t), bc_d(t));
      break;
    case BC_MOV:
      printf("MOV: r%d <- r%d\n", bc_a(t), bc_b(t));
      break;
    case BC_ADD:
      printf("ADD: r%d <- r%d + r%d\n", bc_a(t), bc_b(t), bc_c(t));
      break;
    case BC_SUB:
      printf("SUB: r%d <- r%d - r%d\n", bc_a(t), bc_b(t), bc_c(t));
      break;
    case BC_MUL:
      printf("MUL: r%d <- r%d * r%d\n", bc_a(t), bc_b(t), bc_c(t));
      break;
    case BC_NEG:
      printf("NEG: r%d <- - r%d\n", bc_a(t), bc_b(t));
      break;
    default:
      printf("UNKNOWN\n");
      break;
  }
}

// a bytecode program

typedef struct {
  i64 len;
  bc_t code[];
} bc_prog_t;

static void bc_prog_show(bc_prog_t * t) {
  i64 len = t->len;

  for (i64 i = 0; i < len; i ++) {
    bc_show(t->code[i]);
  }
}
 
// bc_buf

typedef struct {
  i64 len;
  i64 capacity;
  bc_t * code;
} bc_buf_t;

static void bc_buf_init(bc_buf_t * t) {
  t->len = 0;
  t->capacity = 0;
  t->code = NULL;
}

static void bc_buf_add(bc_buf_t * t, bc_t bc) {
  if (t->len + 1 > t->capacity) {
    i64 old_capacity = t->capacity;
    i64 new_capacity = old_capacity < 8 ? 8 : 2 * old_capacity;

    bc_t * old_code = t->code;
    bc_t * new_code = mm_alloc(new_capacity * sizeof(bc_t));

    if (old_code) {
      memcpy(new_code, old_code, old_capacity * sizeof(bc_t));
    }

    t->capacity = new_capacity;
    t->code = new_code;
  }

  t->code[t->len ++] = bc;
}

static bc_prog_t * bc_buf_prog(bc_buf_t * t) {
  i64 len = t->len;

  bc_prog_t * prog = mm_alloc(sizeof(bc_prog_t) + sizeof(bc_t) * len);

  prog->len = len;

  for (i64 i = 0; i < len; i ++) {
    prog->code[i] = t->code[i];
  }

  return prog;
}
