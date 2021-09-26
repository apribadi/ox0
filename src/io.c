// input and output

static char * io_read(char * path) {
  FILE * file = fopen(path, "rb");
  if (!file) panic("io_read: fopen failed!");
  fseek(file, 0, SEEK_END);
  i64 n = ftell(file);
  rewind(file);
  char * buf = mm_alloc(n + 1);
  if ((i64) fread(buf, 1, n, file) != n) panic("io_read: fread failed!");
  fclose(file);
  buf[n] = '\0';
  return buf;
}
