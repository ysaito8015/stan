functions {
  real fib_lpdf(real y);
  real fib_lpdf(real y) {
    real prob;
    if (y <= 2.0)
      prob =  1.0;
    else
      prob = fib_lpdf(y - 1.0) + fib_lpdf(y - 2.0);
    return prob;
  }
}
parameters {
  real y;
}
model {
  y ~ fib();
}
