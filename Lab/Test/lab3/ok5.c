struct A {
  int x;
  int y;
};

int main() {
  struct A m[2];
  m[1].x = read();
  m[0].y = read();
  write(m[0].y + m[1].x);
  return 0;
}
