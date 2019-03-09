functions {
  real fib_lpmf(int n, real y);
  real fib_lpmf(int n, real y) {
    real prob;
    if (n <= 2)
      prob =  1.0;
    else
      prob = fib_lpmf(n - 1 | y) + fib_lpmf(n - 2 | y);
    return log(prob);
  }
}
transformed data {
  int n = 5;
}
parameters {
  real y;
}
model {
  y ~ normal(0,1);
  n ~ fib(y);
}
