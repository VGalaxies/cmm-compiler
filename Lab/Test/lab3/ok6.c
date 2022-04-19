struct A {
  int a;
  int b[2];
  struct B {
    int c;
    int d;
  } e;
};

int main() {
  struct A m[2];
  m[1].b[1] = read();
  write(m[1].b[1]);
  m[0].e.c = read();
  write(m[0].e.c);
  return 0;
}
