typedef struct {
  char const * start;
  char const * stop;
} Symbol;

static Symbol symbol_make(char const * start, char const * stop) {
  return (Symbol) { start, stop };
}

static void print_symbol(Symbol t) {
  printf("%.*s", (int) (t.stop - t.start), t.start);
}
