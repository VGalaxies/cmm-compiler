int main() {
  int a[10];
  int i = 0;
  int j = 0;
  while (i < 10) {
    a[i] = i;
    i = i + 1;
  }
  while (j < 10) {
    if (j < 5) {
      write(a[j] + 5);
    }
    j = j + 1;
  }
  return 0;
}
