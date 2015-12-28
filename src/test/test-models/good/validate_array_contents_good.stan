transformed data {
  int a_ar_int_dim1[3];
  real a_ar_real_dim1[3];
  real x;
  real y;
  real z;
  vector[3] a_ar_vec_dim1[5];
  vector[4] x_v;
  vector[5] y_v;
  vector[6] z_v;

  row_vector[3] a_ar_rowvec_dim1[6]; 
  row_vector[3] x_rv;
  row_vector[3] y_rv;
  row_vector[3] z_rv;

  matrix[2,2] a_ar_mat_dim1[3];
  matrix[2,2] a_ar_mat_dim2[2,2]; 
  matrix[2,2] x_m;
  matrix[2,2] y_m;
  matrix[2,2] z_m;
  
  a_ar_int_dim1 <- {1, 2, 3};
  a_ar_real_dim1 <- {1.0, 2*2, 3^2};
  a_ar_real_dim1 <- {x,y,z};

  a_ar_vec_dim1 <- {x_v,y_v,z_v};

  a_ar_rowvec_dim1 <- {x_rv,y_rv,z_rv};
  a_ar_rowvec_dim1 <- {x_rv,y_rv};

  a_ar_mat_dim1 <- {x_m,y_m,z_m};
  a_ar_mat_dim2 <- { {x_m,y_m} ,{x_m,z_m}};
}
parameters {
  real a;
}
transformed parameters {
  real b_ar_real_dim1[3];
  real x_tp;
  real y_tp;
  real z_tp;

  vector[3] b_ar_vec_dim1[5];
  vector[4] x_tp_v;
  vector[5] y_tp_v;
  vector[6] z_tp_v;

  row_vector[3] b_ar_rowvec_dim1[6]; 
  row_vector[3] x_tp_rv;
  row_vector[3] y_tp_rv;
  row_vector[3] z_tp_rv;

  matrix[2,2] b_ar_mat_dim1[3];
  matrix[2,2] b_ar_mat_dim2[2,2]; 
  matrix[2,2] x_tp_m;
  
  b_ar_vec_dim1 <- {x_tp_v,y_tp_v,z_tp_v};
  b_ar_rowvec_dim1 <- {x_tp_rv,y_tp_rv,z_tp_rv};

  b_ar_mat_dim1 <- {x_tp_m};
  b_ar_mat_dim2 <- { {x_tp_m} };

  b_ar_real_dim1 <- {x_tp,y_tp,z_tp};
  b_ar_real_dim1 <- {1.0, 2*2, 3^2};


}
model {
  a ~ normal(0,1);
}
generated quantities {
  real c_ar_real_dim1[3];
  real x_gq;
  real y_gq;
  real z_gq;
  
  vector[3] c_ar_vec_dim1[5];
  vector[4] x_gq_v;
  vector[5] y_gq_v;
  vector[6] z_gq_v;

  row_vector[3] c_ar_rowvec_dim1[6]; 
  row_vector[3] x_gq_rv;
  row_vector[3] y_gq_rv;
  row_vector[3] z_gq_rv;

  matrix[2,2] c_ar_mat_dim1[3];
  matrix[2,2] c_ar_mat_dim2[2,2]; 
  matrix[2,2] x_gq_m;
  
  c_ar_vec_dim1 <- {x_gq_v,y_gq_v,z_gq_v};
  c_ar_rowvec_dim1 <- {x_gq_rv,y_gq_rv,z_gq_rv};

  c_ar_mat_dim1 <- {x_gq_m};
  c_ar_mat_dim2 <- { {x_gq_m} };

  c_ar_real_dim1 <- {x_gq,y_gq,z_gq};
  c_ar_real_dim1 <- {1.0, 2*2, 3^2};


}
