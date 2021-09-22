bc_vec_t * test_1(void) {
  i64 len = 6;

  bc_vec_t * program = bc_vec_make(len);

  program->data[0] = bc_imm(0, 3);
  program->data[1] = bc_imm(1, 4);
  program->data[2] = bc_mul(0, 0, 0);
  program->data[3] = bc_mul(1, 1, 1);
  program->data[4] = bc_sub(0, 1, 0);
  program->data[5] = bc_exit();

  return program;
}
