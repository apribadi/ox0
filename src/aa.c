// arena allocator

#define AA_MAX_NUM_CHUNKS 30
#define AA_MIN_CHUNK_SIZE_LOG2 12

typedef struct {
  i64 chunk_capacity;
  char * chunk;
  i64 num_chunks_skipped;
  i64 num_chunks;
  char * chunks[AA_MAX_NUM_CHUNKS];
} aa_t;

static inline aa_t aa_make(void) {
  aa_t t;

  t.chunk_capacity = 0;
  t.chunk = NULL;
  t.num_chunks_skipped = 0;
  t.num_chunks = 0;
  for (i64 i = 0; i < AA_MAX_NUM_CHUNKS; i ++) t.chunks[i] = NULL;

  return t;
}

static void * aa_alloc_slow(aa_t *, i64);

static inline void * aa_alloc(aa_t * t, i64 n) {
  i64 old_chunk_capacity = t->chunk_capacity;
  if ((u64) old_chunk_capacity < (u64) n) return aa_alloc_slow(t, n);
  i64 new_chunk_capacity = (old_chunk_capacity - n) & ~7;
  t->chunk_capacity = new_chunk_capacity;
  return t->chunk + new_chunk_capacity;
}

__attribute__((noinline))
static void * aa_alloc_slow(aa_t * t, i64 n) {
  i64 num_chunks_skipped = t->num_chunks_skipped;
  i64 num_chunks = t->num_chunks;
  i64 chunk_index = num_chunks_skipped + num_chunks;
  i64 chunk_size = (i64) 1 << (AA_MIN_CHUNK_SIZE_LOG2 + chunk_index);

  if (n < 0) panic("aa_alloc: negative size!");
  if (chunk_index >= AA_MAX_NUM_CHUNKS) panic("aa_alloc: too many chunks!");

  char * chunk = malloc(chunk_size);

  if (!chunk) panic("aa_alloc: malloc failed!");

  t->chunk_capacity = chunk_size;
  t->chunk = chunk;
  t->num_chunks = num_chunks + 1;
  t->chunks[chunk_index] = chunk;

  return aa_alloc(t, n);
}

static void aa_clear(aa_t * t) {
  i64 num_chunks = t->num_chunks;

  if (num_chunks == 0) return;

  i64 num_chunks_skipped = t->num_chunks_skipped;
  i64 last_chunk_index = num_chunks_skipped + num_chunks - 1;
  i64 last_chunk_size = (i64) 1 << (AA_MIN_CHUNK_SIZE_LOG2 + last_chunk_index);

  t->chunk_capacity = last_chunk_size;

  if (num_chunks == 1) return;

  for (i64 i = num_chunks_skipped; i < last_chunk_index; i ++) {
    free(t->chunks[i]);
    t->chunks[i] = NULL;
  }

  t->num_chunks_skipped = last_chunk_index;
  t->num_chunks = 1;
}
