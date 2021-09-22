// memory management

// TODO: arena

void * mm_alloc(i64 size) {
  void * p = malloc(size);

  if (!p) {
    panic("allocate: malloc failed!");
  }

  return p;
}
