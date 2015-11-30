parameters {
  real y;
}
model {
  int int_1_a[3];
  int_1_a <- { { 1*1 } };
  y ~ normal(0,1);
}
