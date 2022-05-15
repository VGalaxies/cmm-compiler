int foo(int x) {
  if (x == 0 || x == 1) {
    return 1;
  }
  return foo(x - 1) + foo(x - 2);
}

int main() {
  int i = 0;
  while (i < 5) {
    write(foo(i));
    i = i + 1;
  }
  return 0;
}
