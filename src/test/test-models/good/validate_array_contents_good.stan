transformed data {
  int a_i1[3];
  int b_i1[3];
  int c_i2[2,3];

  real x;
  real y;
  real z;

  real a_r1[3];
  real b_r1[3];
  real c_r2[2,3];

  vector[3] a_v1;
  row_vector[3] a_rv1;
  matrix[2,2] a_m;

  a_i1 <- {1, 2, 3};
  b_i1 <- {1+1, 2*2, 3-3};
  c_i2 <- { {1, 2, 3}, {4, 5, 6} };

  a_r1 <- {x, y, z};
  b_r1 <- {1, 2, 3};
  b_r1 <- {1.0, 2*2, 3^2};
}
parameters {
  real a;
}
transformed parameters {
  real d_r1[3];
  real e_r2[2,3];

  vector[3] d_v1;
  row_vector[3] d_rv1;
  matrix[2,2] d_m;
  
  // assign base type both from data and parameters
  d_r1 <- { x, y, z };   // autodiff on RHS
  d_r1 <- { 1.0, 2.0, 2.0 };  // double on RHS
  d_r1 <- { x, 1.0, 1 };  // mixed RHS
  
}
model {
  a ~ normal(0,1);
}
