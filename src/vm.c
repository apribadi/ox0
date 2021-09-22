#define OX_TRACE 0

typedef struct {
  i64 regs[256];
} vm_t;

enum {
  VM_RESULT_OK,
  VM_RESULT_ERROR,
};

typedef i64 vm_result_t;

static void vm_make(vm_t * t) {
  for (i64 i = 0; i < 256; i ++) {
    t->regs[i] = 0;
  }
}

static void vm_drop(vm_t * t) {
  (void) t;
}

static vm_result_t vm_run(vm_t * t, bc_prog_t * prog) {
  // TODO: Write as a set of mutually recursive functions that tail-call each
  // other, so that we can duplicate the dispatch code.

  bc_t * ip = prog->code;

  while (1) {
    bc_t bc = * (ip ++);
    bc_op_t op = bc_op(bc);

    if (OX_TRACE) { bc_show(bc); }

    switch (op) {
      case BC_OP_RETURN: {
        return VM_RESULT_OK;
      }
      case BC_OP_CONSTANT: {
        t->regs[bc_a(bc)] = (i16) bc_d(bc);
        break;
      }
      case BC_OP_ADD: {
        i64 rb = t->regs[bc_b(bc)];
        i64 rc = t->regs[bc_c(bc)];
        i64 ra = rb + rc;
        t->regs[bc_a(bc)] = ra;
        break;
      }
      case BC_OP_SUB: {
        i64 rb = t->regs[bc_b(bc)];
        i64 rc = t->regs[bc_c(bc)];
        i64 ra = rb - rc;
        t->regs[bc_a(bc)] = ra;
        break;
      }
      case BC_OP_MUL: {
        i64 rb = t->regs[bc_b(bc)];
        i64 rc = t->regs[bc_c(bc)];
        i64 ra = rb * rc;
        t->regs[bc_a(bc)] = ra;
        break;
      }
    }
  }
}
