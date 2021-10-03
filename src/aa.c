// arena allocator

typedef struct {
  char * start;
  char * stop;
  i64 chunk_index_start;
  i64 chunk_index_stop;
  char * chunks[36];
} aa_t;

static inline aa_t aa_make(void) {
  aa_t t;

  t.start = NULL;
  t.stop = NULL;
  t.chunk_index_start = 0;
  t.chunk_index_stop = 0;
  for (i64 i = 0; i < 36; i ++) t.chunks[i] = NULL;

  return t;
}

static void * aa_alloc_slow_path(aa_t *, i64);

static inline void * aa_alloc(aa_t * t, i64 n) {
  char * start = t->start;
  char * stop = t->stop;
  if ((u64) (stop - start) < (u64) n) return aa_alloc_slow_path(t, n);
  start = start + ((-n) & ~7);
  t->start = start;
  return start;
}

__attribute__((noinline))
static void * aa_alloc_slow_path(aa_t * t, i64 n) {
  if (n < 0) panic("aa_alloc: negative allocation size!");

  i64 k = t->chunk_index_stop;
  u64 m = 1ull << (12 + k);

  char * start = malloc(m);
  if (!start) panic("aa_alloc: malloc failed!");

  t->start = start;
  t->stop = start + m;
  t->chunk_index_stop = k + 1;
  t->chunks[k] = start;

  return aa_alloc(t, n);
}

static void aa_clear(aa_t * t) {
  i64 a = t->chunk_index_start;
  i64 b = t->chunk_index_stop;

  if (a == b) return;

  i64 k = b - 1;
  u64 m = 1ull << (12 + k);

  for (i64 i = a; i < k; i ++) {
    free(t->chunks[i]);
    t->chunks[i] = NULL;
  }

  char * start = t->chunks[k];

  t->start = start;
  t->stop = start + m;
  t->chunk_index_start = k;
}
