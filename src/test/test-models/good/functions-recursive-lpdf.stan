functions {
  real fib_lpdf(real x, real y);
  real fib_lpdf(real x, real y) {
    real prob;
    if (x <= 2.0)
      prob =  1.0;
    else
      prob = fib_lpdf(x - 1.0 | y) + fib_lpdf(x - 2.0 | y);
    return prob;
  }
}
data {
  real x;
}
parameters {
  real y;
}
model {
  y ~ normal(0,1);
  x ~ fib(y);
}
