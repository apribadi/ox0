// bc_op

typedef u8 bc_op_t;

enum {
  BC_OP_RETURN,
  BC_OP_CONSTANT, // A = dst, D = immediate i16, sign extended
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
static void bc_show(bc_t t) {
  switch (bc_op(t)) {
    case BC_OP_RETURN:
      printf("RETURN\n");
      break;
    case BC_OP_CONSTANT:
      printf("CONSTANT\n");
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

static void bc_buf_init(bc_buf_t * t) {
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
