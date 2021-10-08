// s-expressions

enum {
  SX_TAG_ATOM,
  SX_TAG_LIST,
};

struct sx_t;

typedef struct { i64 len; char const * data; } sx_atom_t;

typedef struct { i64 len; struct sx_t * data; } sx_list_t;

typedef struct sx_t {
  u8 tag;
  union { sx_atom_t atom; sx_list_t list; } as;
} sx_t;

static inline sx_t sx_make_atom(i64 len, char const * data) {
  sx_t t;

  t.tag = SX_TAG_ATOM;
  t.as.atom.len = len;
  t.as.atom.data = data;

  return t;
};

static inline sx_t sx_make_list(mm_arena_t * arena, i64 len) {
  sx_t t;

  t.tag = SX_TAG_LIST;
  t.as.list.len = len;
  t.as.list.data = mm_arena_alloc(arena, sizeof(sx_t) * len);

  return t;
};

static void sx_show_impl(sx_t t) {
  switch (t.tag) {
    case SX_TAG_ATOM:
      printf("%.*s", (int) t.as.atom.len, t.as.atom.data);
      break;

    case SX_TAG_LIST:
      printf("(");
      i64 n = t.as.list.len;
      for (i64 i = 0; i < n; i ++) {
        sx_show_impl(t.as.list.data[i]);
        if (i + 1 != n) {
          printf(" ");
        }
      }
      printf(")");
      break;
  }
};

static void sx_show(sx_t t) {
  sx_show_impl(t);
  printf("\n");
}
