#ifndef __STAN__GM__ARGUMENTS__STRETCH__ENSEMBLE__HPP__
#define __STAN__GM__ARGUMENTS__STRETCH__ENSEMBLE__HPP__

#include <stan/gm/arguments/unvalued_argument.hpp>

namespace stan {
  
  namespace gm {
    
    class arg_stretch_ensemble: public unvalued_argument {
      
    public:
      
      arg_stretch_ensemble() {
        
        _name = "stretch_ensemble";
        _description = "Ensemble Sampler with Stretch Moves";
        
      }
      
    };
    
  } // gm
  
} // stan

#endif

