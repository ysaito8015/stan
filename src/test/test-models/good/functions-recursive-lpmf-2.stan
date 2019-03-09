functions {
  real fib_lpmf(int n);
  real fib_lpmf(int n) {
    real prob;
    if (n <= 2)
      prob =  1.0;
    else
      prob = fib_lpmf(n - 1) + fib_lpmf(n - 2);
    return prob;
  }
}
transformed data {
  int n = 5;
}
model {
  n ~ fib();
}
