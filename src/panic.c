void panic(const char * message) {
  fprintf(stderr, "%s\n", message);

  exit(1);
}
