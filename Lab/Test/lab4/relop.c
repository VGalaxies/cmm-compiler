int main() {
  int a[2], x, y;
  a[0] = 1;
  x = 1;
  y = 2;
  if (1 < 2)
    write(1);
  if (1 < x)
    write(1);
  if (x < 1)
    write(1);
  if (x < y)
    write(1);
  if (a[0] < 1)
    write(1);
  if (1 < a[0])
    write(1);
  if (a[0] < x)
    write(1);
  if (x < a[0])
    write(1);
  return 0;
}
