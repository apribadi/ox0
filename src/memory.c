// TODO: arena

void * allocate(i64 size) {
  void * p = malloc(size);

  if (!p) {
    panic("allocate: malloc failed!");
  }

  return p;
}
