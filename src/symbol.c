typedef struct {
  char const * start;
  char const * stop;
} Symbol;

static Symbol make_symbol(char const * start, char const * stop) {
  return (Symbol) { .start = start, .stop = stop };
}

static void print_symbol(Symbol t) {
  printf("%.*s", (int) (t.stop - t.start), t.start);
}
