#ifndef STAN_LANG_AST_NODE_QUADRATIC_OPTIMIZER_CONTROL_DEF_HPP
#define STAN_LANG_AST_NODE_QUADRATIC_OPTIMIZER_CONTROL_DEF_HPP

#include <stan/lang/ast.hpp>
#include <string>

namespace stan {
  namespace lang {

    quadratic_optimizer_control::quadratic_optimizer_control() { }

    quadratic_optimizer_control::quadratic_optimizer_control(
      const std::string& H_function_name,
      const std::string& v_function_name,
      const std::string& a_function_name,
      const std::string& b_function_name,
      const expression& theta,
      const expression& delta,
      const expression& delta_int,
      const expression& n,
      const expression& tol)
  : H_function_name_(H_function_name),
    v_function_name_(v_function_name),
    a_function_name_(a_function_name),
    b_function_name_(b_function_name),
    theta_(theta), delta_(delta), delta_int_(delta_int), n_(n), tol_(tol) {
    }

  }
}
#endif
