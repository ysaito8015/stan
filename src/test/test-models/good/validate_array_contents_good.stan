parameters {
  real a;
}
model {
  int int_1_a[3];
  int int_1_b[3];

  int int_2_a[2,3];

  int_1_a <- {1, 2, 3};
  int_1_b <- {1+1, 2*2, 3-3};

  int_2_a <- { {1, 2, 3}, {4, 5, 6} };

  a ~ normal(0,1);
}
