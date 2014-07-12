#include <stan/mcmc/ensemble/walk_move_ensemble.hpp>
#include <boost/random/additive_combine.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/math/distributions.hpp>
#include <test/test-models/no-main/optimization/rosenbrock.cpp>
#include <gtest/gtest.h>

typedef boost::ecuyer1988 rng_t;
typedef rosenbrock_model_namespace::rosenbrock_model Model;

namespace stan {
  
  namespace mcmc {
    
    class mock_ensemble: public walk_move_ensemble<Model,
                                                   rng_t> {
      
    public:
      
      mock_ensemble(Model& m, rng_t& rng, std::ostream* o, std::ostream* e) : 
        walk_move_ensemble<Model,rng_t>
        (m, rng, o, e)
      { this->_name = "Mock Ensemble with Walk Move"; }
      
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

TEST(McmcEnsembleWalkMoveEnsemble, construction) {
  EXPECT_FLOAT_EQ(2*2+1, sampler.get_current_states_size());
  EXPECT_FLOAT_EQ(2, sampler.get_params_size());

  std::vector<Eigen::VectorXd> cur_states = sampler.get_current_states();
  Eigen::VectorXd param_means = sampler.get_params_mean();
  Eigen::VectorXd calc_param_means(2);
  calc_param_means.setZero();

  for (int j = 0; j < sampler.get_params_mean().size(); j++) {
    for (int i = 0; i < sampler.get_current_states().size(); i++)
      calc_param_means(j) += cur_states[i](j) / sampler.get_current_states().size();
  }     

  EXPECT_FLOAT_EQ(calc_param_means(0), param_means(0));
  EXPECT_FLOAT_EQ(calc_param_means(1), param_means(1));
}

TEST(McmcEnsembleWalkMoveEnsemble, construction_chi_square_goodness_of_fit) {
  boost::random::mt19937 rng;
  int N = 10000;
  int K = boost::math::round(2 * std::pow(N, 0.4));
  boost::math::chi_squared mydist(K-1);
  boost::math::uniform_distribution<>dist (-2,2);

  double loc[K - 1];
  for(int i = 1; i < K; i++)
    loc[i - 1] = quantile(dist, i * std::pow(K, -1.0));

  for (int i_ = 0; i_ < sampler.get_current_states_size(); i_++) {
    for (int j_ = 0; j_ < sampler.get_current_states()[i_].size(); j_++) {
      int count = 0;
      int bin [K];
      double expect [K];
      for(int i = 0 ; i < K; i++) {
        bin[i] = 0;
        expect[i] = N / K;
      }

      while (count < N) {
        sampler.initialize_ensemble();
        double a = sampler.get_current_states()[i_](j_);
        int i = 0;
        while (i < K-1 && a > loc[i]) 
          ++i;
        ++bin[i];
        count++;
      }
      
      double chi = 0;
      
      for(int j = 0; j < K; j++)
        chi += ((bin[j] - expect[j]) * (bin[j] - expect[j]) / expect[j]);
      
      EXPECT_TRUE(chi < quantile(complement(mydist, 1e-6)));
    }
  } 
}

TEST(McmcEnsembleWalkMoveEnsemble, transition) {
  Eigen::VectorXd a(2);
  a<<1,-1;
  stan::mcmc::sample s0(a,-1,0.3);
  std::vector<Eigen::VectorXd> init_states = sampler.get_current_states();

  EXPECT_NO_THROW(sampler.transition(s0));
}

TEST(McmcEnsembleWalkMoveEnsemble, write_metric) {
  sampler.write_metric(&output);
  EXPECT_TRUE("# No free parameters for stretch move ensemble sampler\n"
              == output.str());
}

TEST(McmcEnsembleWalkMoveEnsemble, choose_walker) {
  boost::random::mt19937 rng;
  int N = 10000;
  for (int n_walker = 3; n_walker < 20; n_walker++) {
    for (int i_walker = 0; i_walker < n_walker; i_walker++) {
      int K = n_walker;
      boost::math::chi_squared mydist(K-1);
      int loc[K - 1];
      for(int i = 1; i < K; i++)
        loc[i - 1] = i;

      int count = 0;
      int bin [K];
      double expect [K];
      for(int i = 0 ; i < K; i++) {
        bin[i] = 0;
        expect[i] = N / (std::pow(2, n_walker-1) - n_walker) 
          * (std::pow(2, n_walker-2) - 1);
      }
    
      expect[i_walker] = 1;

      while (count < N) {
        std::vector<int> temp = sampler.choose_walkers(i_walker, n_walker);
        for (int k_ = 0; k_ < temp.size(); k_++) {
          int a = temp[k_];
          int i = 0;
          while (i < K-1 && a > loc[i])
            ++i;
          ++bin[i];
        }
        count++;
      }
      
      double chi = 0;
      
      for(int j = 0; j < K; j++)
        chi += ((bin[j] - expect[j]) * (bin[j] - expect[j]) / expect[j]);

      EXPECT_TRUE(chi < quantile(complement(mydist, 1e-6)));
    }
  }
}

TEST(McmcEnsembleWalkMoveEnsemble, mean_walker) {
  sampler.initialize_ensemble();
  std::vector<Eigen::VectorXd> temp = sampler.get_current_states();
  std::vector<int> walker_index;
  walker_index.push_back(1);
  walker_index.push_back(3);
  walker_index.push_back(4);
  walker_index.push_back(5);

  Eigen::VectorXd means = sampler.mean_walkers(walker_index, temp);
  Eigen::VectorXd calc_means(means.size());
  calc_means.setZero();

  for (int i = 0; i < walker_index.size(); i++) {
    for (int j = 0; j < means.size(); j++) 
      calc_means(j) += temp[walker_index[i]-1](j) / walker_index.size();
  }

  for (int i = 0; i < means.size(); i++)
    EXPECT_FLOAT_EQ(calc_means(i), means(i));
}
