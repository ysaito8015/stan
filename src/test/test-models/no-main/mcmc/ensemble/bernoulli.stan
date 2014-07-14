transformed data { 
  int<lower=0,upper=1> y[3];

  y[1] <- 1;
  y[2] <- 0;
  y[3] <- 1;
} 
parameters {
  real<lower=0,upper=1> theta;
} 
model {
  theta ~ beta(1,1);
  for (n in 1:3) 
    y[n] ~ bernoulli(theta);
}
