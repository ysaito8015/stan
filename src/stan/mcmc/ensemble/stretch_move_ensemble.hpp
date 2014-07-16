#ifndef __STAN__MCMC__ENSEMBLE__STRETCH_MOVE_ENSEMBLE_HPP__
#define __STAN__MCMC__ENSEMBLE__STRETCH_MOVE_ENSEMBLE_HPP__

#include <iostream>
#include <vector>
#include <algorithm>

#include <stan/mcmc/ensemble/base_ensemble.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <stan/io/dump.hpp>

namespace stan {
  
  namespace mcmc {
    
    // Ensemble Sampler using Stretch Move
        
    template <class M, class BaseRNG>
    class stretch_move_ensemble: public base_ensemble<M,BaseRNG> {
      
    public:
      
      stretch_move_ensemble(M& m, BaseRNG& rng,
                            std::ostream* o = &std::cout, 
                            std::ostream* e = 0)
        : base_ensemble<M,BaseRNG>(m,rng,o,e) {
        this->name_ = "Ensemble Sampler using Stretch Move";
        this->initialize_ensemble();
      } 

      // index is from 0 to num_walker -1
      // returns walker number (from 1 to num_walker)
      int choose_walker(const int& index, const int& num_walkers) {
        boost::random::uniform_int_distribution<> 
          dist(1, num_walkers - 1);         
        int rand_unif = dist(this->rand_int_);
          if (rand_unif >= index+1)
            rand_unif += 1;
          return rand_unif;
      }
      
      void ensemble_transition(std::vector<Eigen::VectorXd>& cur_states,
                               std::vector<Eigen::VectorXd>& new_states,
                               Eigen::VectorXd& logp,
                               Eigen::VectorXd& accept_prob) {
        for (int i = 0; i < cur_states.size(); i++) {

          //calculate initial logprob for walker i
          double logp0 = this->log_prob(cur_states[i]);

          //generates index of random walker that is not currently being moved
          int num_walkers = cur_states.size();
          int rand_unif = choose_walker(i, num_walkers);

          //generates sample from distribution with pdf 1/sqrt(z) 
          //with domain [1/a,a]
          double z = this->sample_z();

          //proposes new walker position
          new_states[i] = cur_states[rand_unif-1] + z
            * (cur_states[i] - cur_states[rand_unif-1]);

          //calculate new log prob
          logp(i) = this->log_prob(new_states[i]);

          if (boost::math::isnan(logp(i))) 
            logp(i) = std::numeric_limits<double>::infinity();

          //calculate MH accept_prob based on Goodman and Weare paper
          accept_prob(i) = std::pow(z, cur_states[i].size() - 1) 
            * std::exp(logp(i) - logp0);

          accept_prob(i) = accept_prob(i) > 1 ? 1 : accept_prob(i);

          if (this->rand_uniform_() > accept_prob(i)) {
            new_states[i] = cur_states[i];
            logp(i) = logp0;
          }
        }
     }

      void write_metric(std::ostream* o) {
        if(!o) return;
        *o << "# No free parameters for stretch move ensemble sampler" << std::endl;
      }

    };
    
  } // mcmc
  
} // stan


#endif
