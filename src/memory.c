// memory management

// arena

#define ARENA_MAX_NUM_CHUNKS 30
#define ARENA_MIN_CHUNK_SIZE_LOG2 12

typedef struct {
  i64 chunk_capacity;
  char * chunk;
  i64 num_chunks_skipped;
  i64 num_chunks;
  char * chunks[ARENA_MAX_NUM_CHUNKS];
} Arena;

static inline Arena mm_arena_make(void) {
  Arena t;

  t.chunk_capacity = 0;
  t.chunk = NULL;
  t.num_chunks_skipped = 0;
  t.num_chunks = 0;
  for (i64 i = 0; i < ARENA_MAX_NUM_CHUNKS; i ++) t.chunks[i] = NULL;

  return t;
}

static void * mm_arena_alloc_slow(Arena *, i64);

static inline void * mm_arena_alloc(Arena * t, i64 n) {
  i64 old_chunk_capacity = t->chunk_capacity;
  if ((u64) old_chunk_capacity < (u64) n) return mm_arena_alloc_slow(t, n);
  i64 new_chunk_capacity = (old_chunk_capacity - n) & ~7;
  t->chunk_capacity = new_chunk_capacity;
  return t->chunk + new_chunk_capacity;
}

__attribute__((noinline))
static void * mm_arena_alloc_slow(Arena * t, i64 n) {
  i64 num_chunks_skipped = t->num_chunks_skipped;
  i64 num_chunks = t->num_chunks;
  i64 chunk_index = num_chunks_skipped + num_chunks;
  i64 chunk_size = (i64) 1 << (ARENA_MIN_CHUNK_SIZE_LOG2 + chunk_index);

  if (n < 0) panic("mm_arena_alloc: negative size!");
  if (chunk_index >= ARENA_MAX_NUM_CHUNKS) panic("mm_arena_alloc: too many chunks!");

  char * chunk = malloc(chunk_size);

  if (!chunk) panic("mm_arena_alloc: malloc failed!");

  t->chunk_capacity = chunk_size;
  t->chunk = chunk;
  t->num_chunks = num_chunks + 1;
  t->chunks[chunk_index] = chunk;

  return mm_arena_alloc(t, n);
}

static void mm_arena_clear(Arena * t) {
  i64 num_chunks = t->num_chunks;

  if (num_chunks == 0) return;

  i64 num_chunks_skipped = t->num_chunks_skipped;
  i64 last_chunk_index = num_chunks_skipped + num_chunks - 1;
  i64 last_chunk_size = (i64) 1 << (ARENA_MIN_CHUNK_SIZE_LOG2 + last_chunk_index);

  t->chunk_capacity = last_chunk_size;

  if (num_chunks == 1) return;

  for (i64 i = num_chunks_skipped; i < last_chunk_index; i ++) {
    free(t->chunks[i]);
    t->chunks[i] = NULL;
  }

  t->num_chunks_skipped = last_chunk_index;
  t->num_chunks = 1;
}
