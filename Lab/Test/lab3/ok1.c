struct A {
  int x;
  int y;
};

int foo(struct A a, int b) { return a.x + b; }

int main() {
  struct A n;
  int m = read();
  n.x = read();
  write(foo(n, m));
  return 0;
}
