#ifndef __STAN__GM__ARGUMENTS__WALK__ENSEMBLE__HPP__
#define __STAN__GM__ARGUMENTS__WALK__ENSEMBLE__HPP__

#include <stan/gm/arguments/unvalued_argument.hpp>

namespace stan {
  
  namespace gm {
    
    class arg_walk_ensemble: public unvalued_argument {
      
    public:
      
      arg_walk_ensemble() {
        
        _name = "walk_ensemble";
        _description = "Ensemble Sampler with Stretch Moves";
        
      }
      
    };
    
  } // gm
  
} // stan

#endif

