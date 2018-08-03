functions {
  matrix H (vector theta,
            real[] delta,
            int[] delta_int) {
    matrix[2, 2] result;
    return result;
  }

  real v (vector theta,
            real[] delta,
            int[] delta_int) {
    real result;
    return result;
  }

  vector a (vector theta,
            real[] delta,
            int[] delta_int) {
    vector[2] result;
    return result;
  }

  real b (vector theta,
          real[] delta,
          int[] delta_int) {
    real result;
    return result;
  }
}

transformed data {
  vector[2] theta;
  real delta[0];
  int delta_int[0];
  vector[2] x;
  real tol = 1e-3;
  int n_unknowns = 1;
  
  x = quadratic_optimizer(H, v, a, b, theta, delta, delta_int, n_unknowns, tol);
}
parameters {
  vector[2] theta_p;
  real dummy_parameter;
}
transformed parameters {
  vector[2] x_p;
  x_p = quadratic_optimizer(H, v, a, b, theta, delta, delta_int, n_unknowns, tol);
}
model {
  dummy_parameter ~ normal(0, 1);
}
