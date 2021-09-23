// virtual machine

typedef struct {
  i64 ireg[256];
} vm_t;

enum {
  VM_RESULT_OK,
  VM_RESULT_ERROR,
};

typedef i64 vm_result_t;

static void vm_init(vm_t * t) {
  for (i64 i = 0; i < 256; i ++) {
    t->ireg[i] = 0;
  }
}

static void vm_drop(vm_t * t) {
  (void) t;
}

static vm_result_t vm_run(vm_t * t, bc_vec_t * program) {
  // TODO: Write as a set of mutually recursive functions that tail-call each
  // other, so that we can duplicate the dispatch code.

  bc_t * ip = program->data;

  while (1) {
    bc_t w = * (ip ++);

    switch (bc_tag(w)) {
      case BC_EXIT: {
        printf("r0 = %ld\n", (long) t->ireg[0]);
        printf("r1 = %ld\n", (long) t->ireg[1]);
        printf("r2 = %ld\n", (long) t->ireg[2]);
        return VM_RESULT_OK;
      }
      case BC_IMM: {
        t->ireg[bc_a(w)] = (i16) bc_d(w);
        break;
      }
      case BC_MOV: {
        t->ireg[bc_a(w)] = t->ireg[bc_b(w)];
        break;
      }
      case BC_ADD: {
        i64 rb = t->ireg[bc_b(w)];
        i64 rc = t->ireg[bc_c(w)];
        i64 ra = rb + rc;
        t->ireg[bc_a(w)] = ra;
        break;
      }
      case BC_SUB: {
        i64 rb = t->ireg[bc_b(w)];
        i64 rc = t->ireg[bc_c(w)];
        i64 ra = rb - rc;
        t->ireg[bc_a(w)] = ra;
        break;
      }
      case BC_MUL: {
        i64 rb = t->ireg[bc_b(w)];
        i64 rc = t->ireg[bc_c(w)];
        i64 ra = rb * rc;
        t->ireg[bc_a(w)] = ra;
        break;
      }
      case BC_NEG: {
        i64 rb = t->ireg[bc_b(w)];
        i64 ra = - rb;
        t->ireg[bc_a(w)] = ra;
        break;
      }
    }
  }
}
