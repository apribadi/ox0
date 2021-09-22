static char * io_read(char const * path) {
  FILE * file = fopen(path, "rb");
  if (!file) panic("io_read: fopen failed!");

  fseek(file, 0, SEEK_END);
  i64 n = ftell(file);
  rewind(file);
  char * buf = mm_alloc(n + 1);

  i64 m = fread(buf, 1, n, file);
  if (m != n) panic("io_read: fread failed!");

  buf[m] = '\0';
  fclose(file);

  return buf;
}
