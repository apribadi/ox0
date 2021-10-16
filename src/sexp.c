// s-expressions

typedef enum : u8 {
  SEXP_TAG_ATOM,
  SEXP_TAG_LIST,
} SexpTag;

struct Sexp;

typedef struct { i64 len; char const * data; } SexpAtom;

typedef struct { i64 len; struct Sexp * data; } SexpList;

typedef struct Sexp {
  u8 tag;
  union { SexpAtom atom; SexpList list; } as;
} Sexp;

static inline Sexp sexp_make_atom(i64 len, char const * data) {
  Sexp t;

  t.tag = SEXP_TAG_ATOM;
  t.as.atom.len = len;
  t.as.atom.data = data;

  return t;
}

static inline Sexp sexp_make_list(mm_arena_t * arena, i64 len) {
  Sexp t;

  t.tag = SEXP_TAG_LIST;
  t.as.list.len = len;
  t.as.list.data = mm_arena_alloc(arena, sizeof(Sexp) * len);

  return t;
}

static void sexp_show_impl(Sexp t) {
  switch (t.tag) {
    case SEXP_TAG_ATOM:
      printf("%.*s", (int) t.as.atom.len, t.as.atom.data);
      break;

    case SEXP_TAG_LIST:
      printf("(");
      i64 n = t.as.list.len;
      for (i64 i = 0; i < n; i ++) {
        sexp_show_impl(t.as.list.data[i]);
        if (i + 1 != n) {
          printf(" ");
        }
      }
      printf(")");
      break;
  }
}

static void sexp_show(Sexp t) {
  sexp_show_impl(t);
  printf("\n");
}
