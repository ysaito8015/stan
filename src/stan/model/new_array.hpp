#ifndef STAN_MODEL_NEW_ARRAY_HPP
#define STAN_MODEL_NEW_ARRAY_HPP

#include <vector>

namespace stan {
  namespace model {

    template <typename T>
    struct new_array {
      std::vector<T> elts_;
      new_array() : elts_() { }
      new_array& add(const T& x) {
        elts_.push_back(x);
        return *this;
      }
      std::vector<T> array() const {
        return elts_;
      }
    };
  }
}


#endif

