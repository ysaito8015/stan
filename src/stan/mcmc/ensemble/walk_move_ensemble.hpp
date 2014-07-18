#ifndef __STAN__MCMC__ENSEMBLE__WALK_MOVE_ENSEMBLE_HPP__
#define __STAN__MCMC__ENSEMBLE__WALK_MOVE_ENSEMBLE_HPP__

#include <iostream>
#include <vector>
#include <algorithm>

#include <stan/mcmc/ensemble/base_ensemble.hpp>
#include <stan/prob/distributions/univariate/discrete/bernoulli.hpp>
#include <stan/prob/distributions/univariate/continuous/normal.hpp>
#include <stan/io/dump.hpp>

namespace stan {
  
  namespace mcmc {
    
    // Ensemble Sampler using Walk Move
        
    template <class M, class BaseRNG>
    class walk_move_ensemble: public base_ensemble<M,BaseRNG> {
      
    public:
      
      walk_move_ensemble(M& m, BaseRNG& rng,
                         std::ostream* o = &std::cout, 
                         std::ostream* e = 0)
        : base_ensemble<M,BaseRNG>(m,rng,o,e) {
        this->name_ = "Ensemble Sampler using Walk Move";
        this->initialize_ensemble();
      } 

      // index is from 0 to num_walkers - 1
      // returns vector from 1 to num_walkers
      std::vector<int> choose_walkers(const int& index, const int& num_walkers) {
        std::vector<int> walkers;

        while (walkers.size() <= 1) {
          walkers.resize(0);
          for (int i = 0; i < num_walkers - 1; i++) {
            int temp = stan::prob::bernoulli_rng(0.5,this->rand_int_);
            if (temp) {
              if (i >= index)
                walkers.push_back(i+2);
              else
                walkers.push_back(i+1);
            }
          }
        }

        return walkers;
      }

      // walker_index is a vector of values ranging from 1 to num_walkers
      Eigen::VectorXd mean_walkers(const std::vector<int>& walker_index, 
                                   const std::vector<Eigen::VectorXd>& cur_states) {

        Eigen::VectorXd mean(cur_states[0].size());
        mean.setZero();

        for (int i = 0; i < mean.size(); i++) {
          for (int j = 0; j < walker_index.size(); j++)
            mean[i] += cur_states[walker_index[j]-1](i) / walker_index.size();
        }
        
        return mean;
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
          std::vector<int> rand_walkers = choose_walkers(i, num_walkers);
          Eigen::VectorXd mean_rand_walkers = mean_walkers(rand_walkers, cur_states);

          //proposes new walker position
          new_states[i] = cur_states[i];
          for (int j = 0; j < rand_walkers.size(); j++)
            new_states[i] += stan::prob::normal_rng(0.0, 1.0, this->rand_int_)
              * (cur_states[rand_walkers[j]-1] - mean_rand_walkers);

          //calculate new log prob
          logp(i) = this->log_prob(new_states[i]);

          if (boost::math::isnan(logp(i))) 
            logp(i) = std::numeric_limits<double>::infinity();

          //calculate MH accept_prob based on Goodman and Weare paper
          accept_prob(i) = std::exp(logp(i) - logp0);

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
