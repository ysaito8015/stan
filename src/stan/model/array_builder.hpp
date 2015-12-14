#ifndef STAN_MODEL_ARRAY_BUILDER_HPP
#define STAN_MODEL_ARRAY_BUILDER_HPP

#include <vector>

namespace stan {
  namespace model {

    template <typename T>
    struct array_builder {
      std::vector<typename T::value_type> elts_;
      array_builder() : elts_() { }
      array_builder& add(const typename T::value_type& x) {
        elts_.push_back(x);
        return *this;
      }
      T array() const {
        return elts_;
      }
    };
  }
}


#endif

