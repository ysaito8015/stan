#include <gtest/gtest.h>
#include <test/unit/lang/utility.hpp>

TEST(lang_parser, quadratic_optimizer_good) {
  test_parsable("quadratic_optimizer_good");
}

    //   // test data-only variables do not have parameters (int locals OK)
    //   if (has_var(quad_fun.delta_, var_map)) {
    //     error_msgs << "sixth argument to quadratic_optimizer"
    //                << " (real data)"
    //                << " must be data only and not reference parameters"

    //   if (has_var(quad_fun.tol_, var_map)) {
    //     error_msgs << "ninth argument to quadratic optimizer"
    //                << " (tolerance)"
    //                << " must be data only and not reference parameters"
    //                << std::endl;


TEST(lang_parser, quadratic_optimizer_bad) {
  test_throws("quadratic_optimizer/bad_fun_H1",
              "first argument to quadratic_optimizer must be ");

  test_throws("quadratic_optimizer/bad_fun_H2",
              "first argument to quadratic_optimizer must be ");

  test_throws("quadratic_optimizer/bad_fun_v1",
              "second argument to quadratic_optimizer must be ");

  test_throws("quadratic_optimizer/bad_fun_v2",
              "second argument to quadratic_optimizer must be ");

  test_throws("quadratic_optimizer/bad_fun_a1",
              "third argument to quadratic_optimizer must be ");

  test_throws("quadratic_optimizer/bad_fun_a2",
              "third argument to quadratic_optimizer must be ");

  test_throws("quadratic_optimizer/bad_fun_b",
              "fourth argument to quadratic_optimizer must");

  test_throws("quadratic_optimizer/bad_arg5",
              "fifth argument to quadratic_optimizer must");

  test_throws("quadratic_optimizer/bad_arg6",
              "sixth argument to quadratic_optimizer must");

  test_throws("quadratic_optimizer/bad_data_arg6",
              "sixth argument to quadratic_optimizer (real data) must");

  test_throws("quadratic_optimizer/bad_arg7",
              "seventh argument to quadratic_optimizer must");

  test_throws("quadratic_optimizer/bad_arg8",
              "eigth argument to quadratic_optimizer must");

  test_throws("quadratic_optimizer/bad_arg9",
              "ninth argument to quadratic_optimizer must");

  test_throws("quadratic_optimizer/bad_data_arg9",
              "ninth argument to quadratic_optimizer (tolerance) must");
  
}
