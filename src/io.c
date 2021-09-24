// input and output

static u8 * io_read(char const * path, i64 m /* number of null bytes appended to file */) {
  FILE * file = fopen(path, "rb");
  if (!file) panic("io_read: fopen failed!");

  fseek(file, 0, SEEK_END);
  i64 n = ftell(file);
  rewind(file);
  u8 * buf = mm_alloc(n + m);

  if ((i64) fread(buf, 1, n, file) != n) panic("io_read: fread failed!");
  fclose(file);

  for (i64 i = 0; i < m; i ++) buf[n + i] = '\0';

  return buf;
}
