struct Operands {
  int o1;
  int o2;
};

int add_twice(struct Operands tmp) { return tmp.o1 + tmp.o2; }

int add(struct Operands temp) { return add_twice(temp); }

int main() {
  int n;
  struct Operands op;
  op.o1 = 1;
  op.o2 = 2;
  n = add(op);
  write(n);
  return 0;
}
