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
          _model(m),
          _params_mean(m.num_params_r()),
          _current_states(2*m.num_params_r()+1),
          _new_states(2*m.num_params_r()+1),
          _logp(2*m.num_params_r()+1),
          _accept_prob(2*m.num_params_r()+1),
          _rand_int(rng),
          _rand_uniform(_rand_int),
          _scale(2.0) {};  
 
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
        for(size_t i = 0; i < _params_mean.size(); ++i)
          values.push_back(_params_mean(i));
       }
      
      void get_param_names(std::vector<std::string>& model_names,
                           std::vector<std::string>& names) {
        for(size_t i = 0; i < _params_mean.size(); ++i)
          names.push_back(model_names[i]);
       }

      void write_sampler_param_names(std::ostream& o) {
        o << "scale__,";
      }
      
      void write_sampler_params(std::ostream& o) {
        o << this->_scale << ",";
      }
      
      void get_sampler_param_names(std::vector<std::string>& names) {
        names.push_back("scale__");
      }
      
      void get_sampler_params(std::vector<double>& values) {
        values.push_back(this->_scale);
      }
      
      void set_scale(const double e) {
        if(e > 0) _scale = e;
      }
      
      double get_scale() { return this->_scale; }
      
      double sample_z() {
        return pow(stan::prob::uniform_rng(0.0, 1.0, _rand_int) * (_scale - 1.0)
                   + 1, 2) / _scale;
      }

      Eigen::VectorXd unconstrain_params(Eigen::VectorXd& params) {
        std::vector<std::string> names;
        std::vector<std::vector<size_t> > dims;
        _model.template get_param_names(names);
        _model.template get_dims(dims);

        std::map<std::string, std::pair<std::vector<double>, std::vector<size_t> > > vars_r;
        std::map<std::string, std::pair<std::vector<int>, std::vector<size_t> > > vars_i;

        int total_count = 0;

        for (int i = 0; i < names.size(); i++) {
          int count = 1;
          if (dims[i].size() > 0) {
            for (int j = 0; j < dims[i].size(); j++)
              count *= dims[i][j];
          }
          std::vector<double> temp;
          temp.resize(0);
          for (int j = total_count; j < total_count+count; j++)
            temp.push_back(params(j));
          vars_r[names[i]] = std::pair<std::vector<double>, 
                                       std::vector<size_t> >(temp, dims[i]);
          
          total_count += count;
        }
              
        stan::io::var_context_builder _var_context(vars_r, vars_i);
        _model.template transform_inits(_var_context, params);

        return params;
      }

      double log_prob(Eigen::VectorXd& q) {
        try {
          _model.template log_prob<false,true>(q, this->_err_stream);
        } catch (std::domain_error e) {
          this->_write_error_msg(this->_err_stream, e);
          return std::numeric_limits<double>::infinity();
        }
        return _model.template log_prob<false,true>(q, this->_err_stream);
      }

      sample transition(sample& init_sample) {
        _params_mean.setZero();
        _logp.setZero();
        _accept_prob.setZero();

        for (int i = 0; i < _new_states.size(); i++) 
          _new_states[i].setZero();

        ensemble_transition(_current_states, _new_states, _logp, _accept_prob);

        for (int i = 0; i < _current_states.size(); i++) {
          Eigen::VectorXd temp_values;
          _model.template write_array(_rand_int, _new_states[i], temp_values);
          for (int j = 0; j < _params_mean.size(); j++) {
            _params_mean(j) += temp_values(j) / _current_states.size();   
          }
        }

        _params_mean = unconstrain_params(_params_mean);
        _current_states = _new_states;

        return sample(_params_mean, _logp.mean(), _accept_prob.mean());
      }

      void initialize_ensemble() {
        for (int i = 0; i < _current_states.size(); i++) {
          _current_states[i].resize(_params_mean.size());
          _new_states[i].resize(_params_mean.size());
          for (int j = 0; j < _current_states[i].size(); j++) {
            _current_states[i](j) = stan::prob::uniform_rng(-2.0,2.0,_rand_int);
          }
        }

        _params_mean.setZero();
        for (int i = 0; i < _current_states.size(); i++) {
          Eigen::VectorXd temp_values;
          temp_values.setZero();
          _model.template write_array(_rand_int, _current_states[i], temp_values);
          for (int j = 0; j < _params_mean.size(); j++) {
            _params_mean(j) += temp_values(j) / _current_states.size();   
          }
        }

        _params_mean = unconstrain_params(_params_mean);
      }


    protected:
      M _model;

      Eigen::VectorXd _params_mean;
      std::vector<Eigen::VectorXd> _current_states;
      std::vector<Eigen::VectorXd> _new_states;

      Eigen::VectorXd _logp;
      Eigen::VectorXd _accept_prob;

      BaseRNG& _rand_int;
      boost::uniform_01<BaseRNG&> _rand_uniform;                

      double _scale;

      void _write_error_msg(std::ostream* error_msgs,
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
