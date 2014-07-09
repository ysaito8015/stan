#include <stan/mcmc/ensemble/base_ensemble.hpp>
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

TEST(McmcEnsembleBaseEnsemble, construction) {
  EXPECT_FLOAT_EQ(2, sampler.get_params_size());
  EXPECT_FLOAT_EQ(2*2+1, sampler.get_current_states_size());
  EXPECT_FLOAT_EQ(2*2+1, sampler.get_new_states_size());
  EXPECT_FLOAT_EQ(2*2+1, sampler.get_logp_size());
  EXPECT_FLOAT_EQ(2*2+1, sampler.get_accept_prob_size());
  EXPECT_FLOAT_EQ(2, sampler.get_scale_init());
  EXPECT_EQ("", output.str());
  EXPECT_EQ("", error.str());
}

TEST(McmcEnsembleBaseEnsemble, write_sampler_state) {
  sampler.write_sampler_state(&output);
  EXPECT_TRUE("# Scale = 2\n" == output.str());
}

TEST(McmcEnsembleBaseEnsemble, get_sampler_diagnostic_names) {
  std::vector<std::string> test_model_names;
  std::vector<std::string> test_names;
  test_model_names.push_back("x");
  test_model_names.push_back("y");

  sampler.get_sampler_diagnostic_names(test_model_names, test_names);
  EXPECT_FLOAT_EQ(2, test_names.size());
  EXPECT_FLOAT_EQ(2, test_model_names.size());

  EXPECT_TRUE("x" == test_model_names[0]);
  EXPECT_TRUE("y" == test_model_names[1]);
  EXPECT_TRUE("x" == test_names[0]);
  EXPECT_TRUE("y" == test_names[1]);
}

TEST(McmcEnsembleBaseEnsemble, get_sampler_diagnostics) {
  std::vector<double> test_values;

  sampler.get_sampler_diagnostics(test_values);
  EXPECT_FLOAT_EQ(2, test_values.size());
}

TEST(McmcEnsembleBaseEnsemble, get_params) {
  std::vector<double> test_values;

  sampler.get_params(test_values);
  EXPECT_FLOAT_EQ(2, test_values.size());
}

TEST(McmcEnsembleBaseEnsemble, get_params_names) {
  std::vector<std::string> test_model_names;
  std::vector<std::string> test_names;
  test_model_names.push_back("x");
  test_model_names.push_back("y");

  sampler.get_param_names(test_model_names, test_names);
  EXPECT_FLOAT_EQ(2, test_names.size());
  EXPECT_FLOAT_EQ(2, test_model_names.size());

  EXPECT_TRUE("x" == test_model_names[0]);
  EXPECT_TRUE("y" == test_model_names[1]);
  EXPECT_TRUE("x" == test_names[0]);
  EXPECT_TRUE("y" == test_names[1]);
}

TEST(McmcEnsembleBaseEnsemble, write_sampler_param_names) {
  std::stringstream output2;

  sampler.write_sampler_param_names(output2);
  EXPECT_TRUE("scale__,"==output2.str());
}

TEST(McmcEnsembleBaseEnsemble, write_sampler_params) {
  std::stringstream output3;

  sampler.write_sampler_params(output3);
  EXPECT_TRUE("2,"==output3.str());
}

TEST(McmcEnsembleBaseEnsemble, get_sampler_param_names) {
  std::vector<std::string> test_names;

  sampler.get_sampler_param_names(test_names);
  EXPECT_FLOAT_EQ(1, test_names.size());
  EXPECT_TRUE("scale__" == test_names[0]);
}

TEST(McmcEnsembleBaseEnsemble, get_sampler_params) {
  std::vector<double> values;

  sampler.get_sampler_params(values);
  EXPECT_FLOAT_EQ(1, values.size());
  EXPECT_FLOAT_EQ(2, values[0]);
}

TEST(McmcEnsembleBaseEnsemble, get_scale) {
  EXPECT_FLOAT_EQ(2, sampler.get_scale());
}

TEST(McmcEnsembleBaseEnsemble, set_scale) {
  EXPECT_FLOAT_EQ(2, sampler.get_scale());
  EXPECT_NO_THROW(sampler.set_scale(3));
  EXPECT_FLOAT_EQ(3, sampler.get_scale());
}

TEST(McmcEnsembleBaseEnsemble, sample_z) {
  boost::random::mt19937 rng;
  int N = 10000;
  int K = boost::math::round(2 * std::pow(N, 0.4));
  boost::math::chi_squared mydist(K-1);
  sampler.set_scale(2);

  double loc[K - 1];
  for(int i = 1; i < K; i++)
    loc[i - 1] = std::pow(i * std::pow(K, -1.0)*(sampler.get_scale()-1)+1,2)
      /sampler.get_scale();

  int count = 0;
  int bin [K];
  double expect [K];
  for(int i = 0 ; i < K; i++) {
    bin[i] = 0;
    expect[i] = N / K;
  }

  while (count < N) {
    double a = sampler.sample_z();
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

TEST(McmcEnsembleBaseEnsemble, initialize_ensemble) {
  sampler.initialize_ensemble();

  EXPECT_FLOAT_EQ(2*2+1, sampler.get_current_states_size());
  EXPECT_FLOAT_EQ(2, sampler.get_params_size());

  std::vector<Eigen::VectorXd> cur_states = sampler.get_current_states();
  Eigen::VectorXd param_means = sampler.get_params_mean();
  Eigen::VectorXd calc_param_means(2);
  calc_param_means.setZero();

  for (int j = 0; j < sampler.get_params_size(); j++) {
    for (int i = 0; i < sampler.get_current_states_size(); i++)
       calc_param_means(j) += cur_states[i](j) / sampler.get_current_states_size();
  }     

  EXPECT_FLOAT_EQ(calc_param_means(0), param_means(0));
  EXPECT_FLOAT_EQ(calc_param_means(1), param_means(1));
}

TEST(McmcEnsembleBaseEnsemble, initialize_chi_square_goodness_of_fit) {
  boost::random::mt19937 rng;
  int N = 10000;
  int K = boost::math::round(2 * std::pow(N, 0.4));
  boost::math::chi_squared mydist(K-1);
  boost::math::uniform_distribution<>dist (-0.5,0.5);

  double loc[K - 1];
  for(int i = 1; i < K; i++)
    loc[i - 1] = quantile(dist, i * std::pow(K, -1.0));

  for (int i = 0; i < sampler.get_current_states_size(); i++) {
    for (int j = 0; j < sampler.get_current_states()[i].size(); j++) {
      int count = 0;
      int bin [K];
      double expect [K];
      for(int i = 0 ; i < K; i++) {
        bin[i] = 0;
        expect[i] = N / K;
      }

      while (count < N) {
        sampler.initialize_ensemble();
        double a = sampler.get_current_states()[i](j);
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

TEST(McmcEnsembleBaseEnsemble, transition) {
  Eigen::VectorXd a(2);
  a<<1,-1;
  stan::mcmc::sample s0(a,-1,0.3);
  sampler.initialize_ensemble();
  std::vector<Eigen::VectorXd> initial_states = sampler.get_current_states();
  Eigen::VectorXd initial_param_mean = sampler.get_params_mean();
  stan::mcmc::sample s_avg = sampler.transition(s0);

  for (int i = 0; i < 5; i++) {
    EXPECT_FLOAT_EQ(0.5+i, sampler.get_logp()(i));
    EXPECT_FLOAT_EQ(0.3+i, sampler.get_accept_prob()(i));

    for (int j = 0; j < 2; j++) {
      EXPECT_FLOAT_EQ(sampler.get_current_states()[i](j),
                      sampler.get_new_states()[i](j));
      EXPECT_FLOAT_EQ(initial_states[i](j)+1,
                      sampler.get_new_states()[i](j));
      EXPECT_FLOAT_EQ(initial_param_mean(j)+1, sampler.get_params_mean()(j));
    }
  }

}
