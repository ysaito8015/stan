functions {
  int fib(int n);
  int fib(int n) {
    if (n <= 2)
      return 1;
    else
      return fib(n - 1) + fib(n - 2);
  }
}
model {
  print(fib(5));
}
