bc_prog_t * test_1(void) {
  i64 len = 6;

  bc_prog_t * prog = mm_alloc(sizeof(bc_prog_t) + sizeof(bc_t) * len);

  prog->len = len;

  prog->code[0] = bc_imm(0, 3);
  prog->code[1] = bc_imm(1, 4);
  prog->code[2] = bc_mul(0, 0, 0);
  prog->code[3] = bc_mul(1, 1, 1);
  prog->code[4] = bc_sub(0, 1, 0);
  prog->code[5] = bc_exit();

  return prog;
}
