// input and output

static char * io_read(char const * path) {
  FILE * file = fopen(path, "rb");
  if (!file) panic("io_read: fopen failed!");
  fseek(file, 0, SEEK_END);
  i64 n = ftell(file);
  rewind(file);
  char * buf = malloc(n + 1);
  if (!buf) panic("io_read: malloc failed!");
  if ((i64) fread(buf, 1, n, file) != n) panic("io_read: fread failed!");
  fclose(file);
  buf[n] = '\0';
  return buf;
}
