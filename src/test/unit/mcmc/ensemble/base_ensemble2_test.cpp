#include <stan/mcmc/ensemble/base_ensemble.hpp>
#include <boost/random/additive_combine.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/math/distributions.hpp>
#include <test/test-models/no-main/mcmc/ensemble/funnel.cpp>
#include <gtest/gtest.h>

typedef boost::ecuyer1988 rng_t;
typedef funnel_model_namespace::funnel_model Model;

namespace stan {
  
  namespace mcmc {
    
    class mock_ensemble: public base_ensemble<Model,
                                              rng_t> {
      
    public:
      
      mock_ensemble(Model& m, rng_t& rng, std::ostream* o, std::ostream* e) : 
        base_ensemble<Model,rng_t>
        (m, rng, o, e)
      { this->_name = "Mock Ensemble"; }
      
      void write_metric(std::ostream* o) {}

      void ensemble_transition(std::vector<Eigen::VectorXd>& cur_states,
                               std::vector<Eigen::VectorXd>& new_states,
                               Eigen::VectorXd& logp,
                               Eigen::VectorXd& accept_prob) {
        for (int i = 0; i < 5; i++) {
          logp(i) = 0.5+i;
          accept_prob(i) = 0.3+i;
          for (int j = 0; j < 2; j++)
            new_states[i](j) = cur_states[i](j) + 1;
        }
      }

      int get_params_size() {return this->_params_mean.size();}
      int get_current_states_size() {return this->_current_states.size();}
      int get_new_states_size() {return this->_new_states.size();}
      int get_logp_size() {return this->_logp.size();}
      int get_accept_prob_size() {return this->_accept_prob.size();}
      double get_scale_init() {return this->_scale;}
      
      Eigen::VectorXd get_params_mean() {return this->_params_mean;}
      Eigen::VectorXd get_accept_prob() {return this->_accept_prob;}
      Eigen::VectorXd get_logp() {return this->_logp;}
      std::vector<Eigen::VectorXd> get_current_states() {
        return this->_current_states;
      }
      std::vector<Eigen::VectorXd> get_new_states() {
        return this->_new_states;
      }
    };
    
  }
  
}  

rng_t base_rng(0);
 
std::stringstream output, error;

static const std::string DATA = "";
std::stringstream data_stream(DATA);
stan::io::dump dummy_context(data_stream);

Model model(dummy_context);

stan::mcmc::mock_ensemble sampler(model, base_rng, &output, &error);

TEST(McmcEnsembleBaseEnsemble, funnel_unconstrain_params) {
  Eigen::VectorXd a(11);
  a << 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11;
  sampler.initialize_ensemble();
  Eigen::VectorXd res = sampler.unconstrain_params(a);

  for (int i = 0; i < a.size(); i++)
    EXPECT_FLOAT_EQ(i+1, res(i));
}
