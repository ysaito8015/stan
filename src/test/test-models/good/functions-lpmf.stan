functions {
  real foo_lpmf(int x) {
    return 1.0;
 }
}
data {
  int y;
}
model {
  y ~ foo();
}
