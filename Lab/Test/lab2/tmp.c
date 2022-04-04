struct A {
  int a;
  float b;
};

struct B {
  float c;
  int d;
};

int foo() {
  struct A x;
  struct B y;
  y = x;
}
