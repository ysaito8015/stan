#ifndef __STAN__MCMC__ENSEMBLE__BASE_ENSEMBLE_HPP__
#define __STAN__MCMC__ENSEMBLE__BASE_ENSEMBLE_HPP__

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>

#include <stan/mcmc/base_mcmc.hpp>
#include <stan/prob/distributions/univariate/continuous/normal.hpp>
#include <stan/prob/distributions/univariate/continuous/uniform.hpp>
#include <stan/io/var_context_builder.hpp>

namespace stan {
  
  namespace mcmc {
    
    // Ensemble Sampler
        
    template <class M, class BaseRNG>
    class base_ensemble: public base_mcmc {
      
    public:
      
      base_ensemble(M& m, BaseRNG& rng,
                    std::ostream* o, std::ostream* e)
        : base_mcmc(o,e), 
          model_(m),
          params_mean_(m.num_params_r()),
          current_states_(2*m.num_params_r()+1),
          new_states_(2*m.num_params_r()+1),
          logp_(2*m.num_params_r()+1),
          accept_prob_(2*m.num_params_r()+1),
          rand_int_(rng),
          rand_uniform_(rand_int_),
          scale_(2.0),
          names_(m.num_params_r()),
          dims_(m.num_params_r()) {};  
 
      ~base_ensemble() {};

      virtual void write_metric(std::ostream* o) = 0;
      virtual void ensemble_transition(std::vector<Eigen::VectorXd>& cur_states,
                                       std::vector<Eigen::VectorXd>& new_states,
                                       Eigen::VectorXd& logp,
                                       Eigen::VectorXd& accept_prob) = 0;
   
      void write_sampler_state(std::ostream* o) {
        if(!o) return;
        *o << "# Scale = " << get_scale() << std::endl;
        this->write_metric(o);
      }
      
      void get_sampler_diagnostic_names(std::vector<std::string>& model_names,
                                        std::vector<std::string>& names) {
        this->get_param_names(model_names, names);
      }
      
      void get_sampler_diagnostics(std::vector<double>& values) {
        this->get_params(values);
      }

      void get_params(std::vector<double>& values) {
        for(size_t i = 0; i < params_mean_.size(); ++i)
          values.push_back(params_mean_(i));
       }
      
      void get_param_names(std::vector<std::string>& model_names,
                           std::vector<std::string>& names) {
        for(size_t i = 0; i < params_mean_.size(); ++i)
          names.push_back(model_names[i]);
       }

      void write_sampler_param_names(std::ostream& o) {
        o << "scale__,";
      }
      
      void write_sampler_params(std::ostream& o) {
        o << this->scale_ << ",";
      }
      
      void get_sampler_param_names(std::vector<std::string>& names) {
        names.push_back("scale__");
      }
      
      void get_sampler_params(std::vector<double>& values) {
        values.push_back(this->scale_);
      }
      
      void set_scale(const double e) {
        if(e > 0) scale_ = e;
      }
      
      double get_scale() { return this->scale_; }
      
      double sample_z() {
        return pow(stan::prob::uniform_rng(0.0, 1.0, rand_int_) * (scale_ - 1.0)
                   + 1, 2) / scale_;
      }

      Eigen::VectorXd unconstrain_params(Eigen::VectorXd& params) {

        std::map<std::string, std::pair<std::vector<double>, std::vector<size_t> > > vars_r;
        std::map<std::string, std::pair<std::vector<int>, std::vector<size_t> > > vars_i;

        int total_count = 0;

        for (int i = 0; i < names_.size(); i++) {
          int count = 1;
          if (dims_[i].size() > 0) {
            for (int j = 0; j < dims_[i].size(); j++)
              count *= dims_[i][j];
          }
          std::vector<double> temp;
          temp.resize(0);
          for (int j = total_count; j < total_count+count; j++)
            temp.push_back(params(j));
          vars_r[names_[i]] = std::pair<std::vector<double>, 
                                       std::vector<size_t> >(temp, dims_[i]);
          
          total_count += count;
        }

        stan::io::var_context_builder vars_context(vars_r, vars_i);
        model_.template transform_inits(vars_context, params);

        return params;
      }

      double log_prob(Eigen::VectorXd& q) {
        try {
          model_.template log_prob<false,true>(q, this->_err_stream);
        } catch (std::domain_error e) {
          this->write_error_msg_(this->_err_stream, e);
          return std::numeric_limits<double>::infinity();
        }
        return model_.template log_prob<false,true>(q, this->_err_stream);
      }

      sample transition(sample& init_sample) {
        params_mean_.setZero();
        logp_.setZero();
        accept_prob_.setZero();

        for (int i = 0; i < new_states_.size(); i++) 
          new_states_[i].setZero();

        ensemble_transition(current_states_, new_states_, logp_, accept_prob_);

        for (int i = 0; i < current_states_.size(); i++) {
          Eigen::VectorXd temp_values;
          model_.template write_array(rand_int_, new_states_[i], temp_values);
          for (int j = 0; j < params_mean_.size(); j++) {
            params_mean_(j) += temp_values(j) / current_states_.size();   
          }
        }

        params_mean_ = unconstrain_params(params_mean_);
        current_states_ = new_states_;

        return sample(params_mean_, logp_.mean(), accept_prob_.mean());
      }

      void initialize_ensemble() {
        for (int i = 0; i < current_states_.size(); i++) {
          current_states_[i].resize(params_mean_.size());
          new_states_[i].resize(params_mean_.size());
          for (int j = 0; j < current_states_[i].size(); j++) {
            current_states_[i](j) = stan::prob::uniform_rng(-2.0,2.0,rand_int_);
          }
        }

        params_mean_.setZero();
        for (int i = 0; i < current_states_.size(); i++) {
          Eigen::VectorXd temp_values;
          temp_values.setZero();
          model_.template write_array(rand_int_, current_states_[i], temp_values);
          for (int j = 0; j < params_mean_.size(); j++) {
            params_mean_(j) += temp_values(j) / current_states_.size();   
          }
        }

        model_.template get_param_names(names_);
        model_.template get_dims(dims_);

        params_mean_ = unconstrain_params(params_mean_);
      }


    protected:
      M model_;

      Eigen::VectorXd params_mean_;
      std::vector<Eigen::VectorXd> current_states_;
      std::vector<Eigen::VectorXd> new_states_;

      Eigen::VectorXd logp_;
      Eigen::VectorXd accept_prob_;

      BaseRNG& rand_int_;
      boost::uniform_01<BaseRNG&> rand_uniform_;                

      double scale_;

      std::vector<std::string> names_;
      std::vector<std::vector<size_t> > dims_;

      void write_error_msg_(std::ostream* error_msgs,
                           const std::domain_error& e) {
          if (!error_msgs) return;
          
          *error_msgs << std::endl
                      << "Informational Message: The parameter state is about to be Metropolis"
                      << " rejected due to the following underlying, non-fatal (really)"
                      << " issue (and please ignore that what comes next might say 'error'): "
                      << e.what()
                      << std::endl
                      << "If the problem persists across multiple draws, you might have"
                      << " a problem with an initial state."
                      << std::endl
                      << " If the problem does not persist, the resulting samples will still"
                      << " be drawn from the posterior."
                      << std::endl;
      }

    };
    
  } // mcmc
  
} // stan


#endif
