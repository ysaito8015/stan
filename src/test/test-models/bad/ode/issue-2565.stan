functions {
  real[] rhs(real t,
             real[] y,
             real[] theta,
             real[] x_r,
             int[] x_i) {
    real ka = theta[1];
    real Vmax = theta[2];
    real Km = theta[3];
    real V = theta[4];

    real dose = x_r[1];

    return { (ka * dose * exp(-ka * (t + 2))
              - Vmax * y[1] / (Km + y[1])) / V };
  }

  vector unit_ll(vector shared,
                 vector unit_spec,
                 real[] x_r,
                 int [] x_i) {
    real theta[4] = to_array_1d(unit_spec);
    real sigma = shared[1];

    real y_hat[8, 1] =
      integrate_ode_bdf(rhs, { x_r[17] },
                        x_r[18], x_r[9:16],
                        theta, { x_r[19] }, x_i);

    vector[8] lp;
    for (i in 1:8) {
      lp[i] = normal_lpdf(x_r[i] | log(y_hat[i,1]), sigma);
    }

    return lp;
  }
}

data {
  int<lower=1> n_ind;
  real log_y_obs[n_ind, 8];
  real ts[8];
  real doses[n_ind];
}

transformed data {
  int x_i[n_ind, 0];
  real y_full[n_ind, 8 + 8 + 1 + 1 + 1]; // Observations + Times +
                                         // Initial value + Initial time + Dose

  // Observations
  y_full[,1:8] = log_y_obs;
  // Times
  y_full[,9:16] = to_array_2d(rep_matrix(to_row_vector(ts), n_ind));
  // Initial value
  y_full[,17] = rep_array(0.0, n_ind);
  // Initial time
  y_full[,18] = rep_array(-2.0, n_ind);
  // Dose
  y_full[,19] = doses;
}

parameters {
  real<lower=0> sigma;

  real mu_ka;
  real mu_Vmax;
  real mu_Km;
  real mu_V;

  vector<lower=0>[4] tau;
  matrix[4, n_ind] etas;
}

transformed parameters {
  vector[n_ind] ka;
  vector[n_ind] Vmax;
  vector[n_ind] Km;
  vector[n_ind] V;

  {
    matrix[n_ind, 4] theta =
      exp(rep_matrix([mu_ka, mu_Vmax, mu_Km, mu_V]', n_ind)
          + diag_pre_multiply(tau, etas))';

    ka = theta[,1];
    Vmax = theta[,2];
    Km = theta[,3];
    V = theta[,4];
  }
}

model {
  vector[1] shared = [ sigma ]';
  vector[4] unit_spec[n_ind];

  for (i in 1:n_ind) {
    unit_spec[i] = [
      ka[i], Vmax[i], Km[i], V[i]
    ]';
  }

  target += map_rect(unit_ll, shared, unit_spec, y_full, x_i);

  // priors
  mu_ka ~ student_t(4, 0, 5);
  mu_Vmax ~ student_t(4, 0, 5);
  mu_Km ~ student_t(4, 0, 5);
  mu_V ~ student_t(4, 0, 5);

  sigma ~ cauchy(0, 5);

  to_vector(etas) ~ std_normal();
}
