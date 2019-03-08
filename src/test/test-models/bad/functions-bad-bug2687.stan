functions{
  	
  real poisson_inverse_gaussian_log_original_lpmf(int y, real mu, real beta, real tau);

  real poisson_inverse_gaussian_log_original_lpmf(int y, real mu, real beta, real tau) {
    real exp_mu = exp(mu);
    real exp_mu_beta_tau = exp_mu*beta+tau;
    real exp_mu_beta_tau_exp_mu_2 = exp_mu_beta_tau*exp_mu*2.0;
    real prob;

    if(y==0) prob =
               exp(1/exp_mu_beta_tau*(1-(1+exp_mu_beta_tau_exp_mu_2)^0.5));

    if(y==1) prob =
               exp_mu*(1+exp_mu_beta_tau_exp_mu_2)^-0.5 *
               exp(poisson_inverse_gaussian_log_original_lpmf(0 | mu,beta, tau));

    prob =
      exp_mu_beta_tau_exp_mu_2 /
      (1+exp_mu_beta_tau_exp_mu_2) *
      (1. - 3. /(2.*y)) *
      exp(poisson_inverse_gaussian_log_original_lpmf(y-1 | mu,beta, tau)) +
      exp_mu^2 /
      (1+exp_mu_beta_tau_exp_mu_2) /
      (y*(y-1)) *
      exp(poisson_inverse_gaussian_log_original_lpmf(y-2 | mu,beta, tau));

    return log(prob);
  }
}
data {
  int<lower=0> N;
  int<lower=0> G;
  int<lower=0> counts[N,G];
  real my_prior[2];
  int<lower=0, upper=1> omit_data;

  // Alternative models
  int<lower=0, upper=1> is_prior_asymetric;
}
parameters {

  // Overall properties of the data
  real<lower=0> lambda_mu; // So is compatible with logGamma prior
  real<lower=0> lambda_sigma_raw;
  real<lower=0> sigma;
  real<lower=0> tau;

  // Gene-wise properties of the data
  vector[G] lambda;
}
transformed parameters {
  real<lower=0> lambda_sigma = lambda_sigma_raw;
}
model {

  // Overall properties of the data
  lambda_mu ~ gamma(3,2);
  lambda_sigma_raw ~ normal(0,1);
  sigma ~ normal(0,2);
  tau ~ normal(0,2);


  // Gene-wise properties of the data
  lambda ~ normal(lambda_mu, lambda_sigma);

  // Sample from data
  if(omit_data==0)
    for(n in 1:N)
      for(g in 1:G)
        counts[n,g] ~ poisson_inverse_gaussian_log_original(lambda[g], sigma, tau);
}
