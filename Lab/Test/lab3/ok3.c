int read_wrapper() {
  return read();
}

int add(int a, int b) {
  return a + b;
}

int main() {
  int x = read_wrapper();
  int y = read_wrapper();
  write(add(x, y));
  return 0;
}
