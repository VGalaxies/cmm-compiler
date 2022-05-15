int foo() {
  return 1;
}

int bar(int a) {
  return a + foo();
}

int myadd(int x, int y) {
  return bar(x) + y;
}

int main() {
  write(foo());
  write(bar(1));
  write(myadd(1, 2));
  return 0;
}
