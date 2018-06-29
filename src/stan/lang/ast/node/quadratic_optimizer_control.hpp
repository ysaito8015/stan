#ifndef STAN_LANG_AST_NODE_QUADRATIC_OPTIMIZER_CONTROL_HPP
#define STAN_LANG_AST_NODE_QUADRATIC_OPTIMIZER_CONTROL_HPP

#include <stan/lang/ast/node/expression.hpp>
#include <string>

namespace stan {
  namespace lang {

    struct expression;

    /**
     * Structure for algebraic solver statement.
     */
    struct quadratic_optimizer_control {
      /**
       * Name of the matrix function in quadratic target function.
       */
      std::string H_function_name_;

      /**
       * Name of the vector function (linear term) in the target function.
       */
      std::string v_function_name_;

      /**
       * Name of the constraint vector function (linear constraint)
       */
      std::string a_function_name_;

      /**
       * Name of the constraint scalar function (constant constraint)
       */
      std::string b_function_name_;

      /**
       * Parameters
       */
      expression theta_;

      /**
       * Real-valued data
       */
      expression delta_;

      /**
       * Interger-valued data
       */
      expression delta_int_;

      /**
       * Number of unknowns we solve for.
       */
      expression n_;

      /**
       * Tolerance (real)
       */
      expression tol_;

      /**
       * Construct a default quadratic solver node with control.
       */
      quadratic_optimizer_control();

      /**
       * Construct a quadratic optimizer with a control optimizer.
       *
       * @param H_function_name name of matrix function for the target.
       * @param v_function_name name of vector function for the target.
       * @param a_function_name name of linear constraint function.
       * @param b_function_name name of scalar constraint function.
       * @param theta parameters
       * @param delta real-valued data
       * @param delta_int integer-valued data.
       * @param n number of unknowns
       * @param tol tolerance underwhich an element is considered to NOT
       *            be active.
       */
      quadratic_optimizer_control(const std::string& H_function_name,
                                  const std::string& v_function_name,
                                  const std::string& a_function_name,
                                  const std::string& b_function_name,
                                  const expression& theta,
                                  const expression& delta,
                                  const expression& delta_int,
                                  const expression& n,
                                  const expression& tol);
    };

  }
}
#endif
