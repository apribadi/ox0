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

// vec

typedef struct {
  i64 len;
  bc_t data[];
} bc_vec_t;

static bc_vec_t * bc_vec_make(i64 len) {
  bc_vec_t * t = mm_alloc(sizeof(bc_vec_t) + sizeof(bc_t) * len);

  t->len = len;

  return t;
}

static void bc_vec_show(bc_vec_t * t) {
  i64 len = t->len;

  for (i64 i = 0; i < len; i ++) {
    bc_show(t->data[i]);
  }
}
 
// buf

typedef struct {
  i64 len;
  i64 cap;
  bc_t * data;
} bc_buf_t;

static bc_buf_t bc_buf_make(void) {
  return (bc_buf_t) { .len = 0, .cap = 0, .data = NULL };
}

static void bc_buf_add(bc_buf_t * t, bc_t elt) {
  if (t->len >= t->cap) {
    i64 old_cap = t->cap;
    i64 new_cap = old_cap == 0 ? 8 : 2 * old_cap;

    bc_t * old_data = t->data;
    bc_t * new_data = mm_alloc(new_cap * sizeof(bc_t));

    if (old_data) memcpy(new_data, old_data, old_cap * sizeof(bc_t));

    t->cap = new_cap;
    t->data = new_data;
  }

  i64 old_len = t->len;

  t->len = old_len + 1;
  t->data[old_len] = elt;
}
