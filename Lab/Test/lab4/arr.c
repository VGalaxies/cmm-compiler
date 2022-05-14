int main() {
  int arr[4];
  int x;
  arr[1] = read();
  write(arr[1]);
  x = arr[1];
  write(x);
  arr[1] = 1;
  write(arr[1]);
  arr[1] = x;
  write(arr[1]);
  return 0;
}
