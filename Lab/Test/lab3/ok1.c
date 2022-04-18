struct A {
  int x;
  int y;
};

int add(int temp[2][2]) {
  return temp[0][0] + temp[1][1];
}

int main() {
  int a[2][2];
  struct A z[2];
  z[1].y = read();
  a[1][1] = read();
  write(z[1].y);
  write(a[1][1]);
  return 0;
}
