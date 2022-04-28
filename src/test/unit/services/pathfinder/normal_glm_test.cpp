#include <stan/services/pathfinder/multi.hpp>
#include <stan/io/array_var_context.hpp>
#include <stan/io/empty_var_context.hpp>
#include <test/test-models/good/normal_glm.hpp>
#include <test/unit/services/instrumented_callbacks.hpp>
#include <stan/callbacks/stream_writer.hpp>
#include <test/unit/services/pathfinder/util.hpp>
#include <gtest/gtest.h>

auto&& threadpool_init = stan::math::init_threadpool_tbb(8);

auto init_context() {
  std::fstream stream(
      "/home/steve/stan/origin/stan/src/test/unit/services/pathfinder/"
      "glm_test.data.R",
      std::fstream::in);
  return stan::io::dump(stream);
}

class ServicesPathfinderGLM : public testing::Test {
 public:
  ServicesPathfinderGLM()
      : init(init_ss),
        parameter(parameter_ss),
        diagnostics(diagnostic_ss),
        context(init_context()),
        model(context, 0, &model_ss) {}

  std::stringstream init_ss, parameter_ss, diagnostic_ss, model_ss;
  stan::callbacks::stream_writer init;
  stan::test::values parameter;
  stan::test::values diagnostics;
  stan::io::dump context;
  stan_model model;
};

stan::io::array_var_context init_init_context() {
  std::vector<std::string> names_r{"b", "Intercept", "sigma"};
  std::vector<double> values_r{0, 0, 0, 0, 0, 0, 1};
  using size_vec = std::vector<size_t>;
  std::vector<size_vec> dims_r{size_vec{5}, size_vec{}, size_vec{}};
  std::vector<std::string> names_i{""};
  std::vector<int> values_i{};
  using size_vec = std::vector<size_t>;
  std::vector<size_vec> dims_i{size_vec{}};
  return stan::io::array_var_context(names_r, values_r, dims_r);
}

TEST_F(ServicesPathfinderGLM, single) {
  constexpr unsigned int seed = 0;
  constexpr unsigned int chain = 1;
  constexpr double init_radius = .7;
  constexpr double num_elbo_draws = 80;
  constexpr double num_draws = 100;
  constexpr int history_size = 15;
  constexpr double init_alpha = 1;
  constexpr double tol_obj = 0;
  constexpr double tol_rel_obj = 0;
  constexpr double tol_grad = 0;
  constexpr double tol_rel_grad = 0;
  constexpr double tol_param = 0;
  constexpr int num_iterations = 60;
  constexpr bool save_iterations = false;
  constexpr int num_eval_attempts = 100;
  constexpr int refresh = 0;
  stan::test::mock_callback callback;
  stan::io::empty_var_context empty_context;  // = init_init_context();
  std::ostream empty_ostream(nullptr);
  stan::test::loggy logger(empty_ostream);

  std::vector<std::tuple<Eigen::VectorXd, Eigen::VectorXd>> input_iters;

  int return_code = stan::services::pathfinder::pathfinder_lbfgs_single(
      model, empty_context, seed, chain, init_radius, history_size, init_alpha,
      tol_obj, tol_rel_obj, tol_grad, tol_rel_grad, tol_param, num_iterations,
      save_iterations, refresh, callback, num_elbo_draws, num_draws,
      num_eval_attempts, logger, init, parameter, diagnostics);
  /*
  for (auto&& times : parameter.times_) {
    std::cout << times;
  }
  */
  // Eigen::MatrixXd param_vals = parameter.values_.transpose();
  // Eigen::MatrixXd param_vals = parameter.values_.transpose();
  Eigen::MatrixXd param_vals = std::move(parameter.values_);
  /*
  std::cout << "\n --- Optim Path ---" << std::endl;
  for (Eigen::Index i = 0; i < diagnostics.optim_path_.size(); ++i) {
    Eigen::MatrixXd tmp(2, std::get<0>(diagnostics.optim_path_[i]).size());
    tmp.row(0) = std::get<0>(diagnostics.optim_path_[i]);
    tmp.row(1) = std::get<1>(diagnostics.optim_path_[i]);
    //std::cout << "Iter: " << i << "\n" << tmp << "\n";
  }
  */
  Eigen::IOFormat CommaInitFmt(Eigen::StreamPrecision, 0, ", ", ", ", "\n", "",
                               "", "");

  // std::cout << "---- Results  -------" << std::endl;
  Eigen::VectorXd mean_vals = param_vals.rowwise().mean().eval();
  //       std::cout << "Mean Values: \n" <<
  //       mean_vals.transpose().eval().format(CommaInitFmt) << "\n";
  Eigen::VectorXd sd_vals = (((param_vals.colwise() - mean_vals)
                                  .array()
                                  .square()
                                  .matrix()
                                  .rowwise()
                                  .sum()
                                  .array()
                              / (param_vals.cols() - 1))
                                 .sqrt())
                                .transpose()
                                .eval();
  // std::cout << "\n\n" << param_vals.format(CommaInitFmt) << "\n\n";
  Eigen::MatrixXd prev_param_vals(10, 100);
  prev_param_vals << 0.989133, 0.987283, 1.00865, 1.01761, 1.02141, 0.979206,
      1.0016, 1.02329, 1.01547, 1.00124, 1.00566, 0.982438, 1.01388, 0.990802,
      1.02231, 1.00406, 1.01691, 0.997928, 0.997293, 0.974797, 1.00595, 1.01402,
      0.984706, 0.977346, 1.00434, 0.974, 0.963947, 0.999307, 0.98874, 0.997258,
      0.992295, 1.01447, 0.989742, 1.00031, 1.00598, 0.997586, 0.989764,
      1.00578, 1.00274, 1.00065, 1.00799, 1.01237, 0.96816, 1.0062, 1.01169,
      1.00744, 0.980485, 1.00712, 1.02736, 1.00107, 0.986573, 1.00091, 0.998115,
      0.99281, 0.995292, 1.00851, 0.984376, 0.98788, 1.00403, 0.976502,
      0.982548, 0.992574, 1.00651, 1.01491, 0.985712, 0.990779, 1.00292,
      0.984381, 0.985727, 1.02072, 0.972444, 1.02651, 1.00152, 1.00147, 1.00619,
      1.0108, 1.00412, 1.02858, 1.00949, 0.985685, 1.00205, 0.990218, 0.997355,
      0.994934, 1.01341, 1.01248, 1.01124, 1.00118, 1.01047, 1.00284, 0.978697,
      1.01447, 1.00973, 0.990684, 1.00624, 0.976974, 1.00119, 0.996196,
      0.999012, 0.983142, 0.68016, 0.699344, 0.673045, 0.691876, 0.669649,
      0.672762, 0.716136, 0.697584, 0.674966, 0.650528, 0.696122, 0.687595,
      0.690818, 0.69065, 0.680874, 0.667614, 0.703743, 0.68247, 0.674234,
      0.656675, 0.69866, 0.667602, 0.671322, 0.682141, 0.65578, 0.691154,
      0.690077, 0.661168, 0.682424, 0.686199, 0.669013, 0.671651, 0.662703,
      0.681059, 0.6717, 0.667242, 0.662769, 0.661001, 0.691038, 0.693961,
      0.67983, 0.697614, 0.695461, 0.688362, 0.666684, 0.669772, 0.678979,
      0.695811, 0.701869, 0.699734, 0.666627, 0.663433, 0.657691, 0.693838,
      0.656416, 0.691867, 0.652502, 0.66561, 0.661675, 0.681262, 0.639635,
      0.672269, 0.676344, 0.654824, 0.680483, 0.693304, 0.673858, 0.69701,
      0.669327, 0.670104, 0.700511, 0.712719, 0.68641, 0.676175, 0.673863,
      0.671275, 0.671586, 0.670274, 0.667039, 0.665298, 0.6905, 0.678944,
      0.672464, 0.685418, 0.675042, 0.698088, 0.686032, 0.677391, 0.712156,
      0.68469, 0.697393, 0.669891, 0.663002, 0.677229, 0.677726, 0.668302,
      0.679998, 0.699712, 0.671381, 0.663888, 0.511369, 0.525085, 0.531038,
      0.529463, 0.529189, 0.543642, 0.535529, 0.533005, 0.527877, 0.509799,
      0.536219, 0.519464, 0.513025, 0.540542, 0.502001, 0.511964, 0.507174,
      0.535565, 0.54626, 0.54512, 0.533558, 0.516238, 0.529573, 0.525098,
      0.544148, 0.512651, 0.532813, 0.525611, 0.49472, 0.50992, 0.553442,
      0.536401, 0.532948, 0.502132, 0.513881, 0.492237, 0.524224, 0.517362,
      0.523663, 0.542269, 0.537562, 0.509463, 0.524809, 0.493832, 0.525565,
      0.537415, 0.550462, 0.543065, 0.516061, 0.522298, 0.515901, 0.532414,
      0.532221, 0.523204, 0.552887, 0.526091, 0.530968, 0.530605, 0.51651,
      0.528895, 0.532424, 0.536296, 0.532983, 0.548964, 0.535942, 0.504232,
      0.563204, 0.515654, 0.536047, 0.53497, 0.517214, 0.525378, 0.518352,
      0.516509, 0.522678, 0.543605, 0.508595, 0.511166, 0.517616, 0.526743,
      0.522237, 0.54086, 0.560177, 0.52307, 0.509021, 0.504419, 0.538517,
      0.524971, 0.533303, 0.514497, 0.515468, 0.522429, 0.523632, 0.5504,
      0.516653, 0.520695, 0.520476, 0.531572, 0.537675, 0.514285, 0.30942,
      0.314056, 0.322901, 0.307963, 0.29414, 0.268925, 0.312988, 0.314239,
      0.298855, 0.308249, 0.290594, 0.291957, 0.275043, 0.311282, 0.313345,
      0.32358, 0.309169, 0.277428, 0.320151, 0.327048, 0.314795, 0.297986,
      0.305475, 0.318588, 0.270818, 0.29099, 0.309577, 0.296313, 0.330638,
      0.295411, 0.295335, 0.308153, 0.286721, 0.326388, 0.289772, 0.317589,
      0.311338, 0.300116, 0.306695, 0.317983, 0.296273, 0.32718, 0.305991,
      0.297433, 0.332018, 0.311691, 0.315756, 0.285922, 0.293896, 0.303194,
      0.313389, 0.292248, 0.276507, 0.303205, 0.285911, 0.30499, 0.313411,
      0.302989, 0.290302, 0.317428, 0.290353, 0.301325, 0.304949, 0.307035,
      0.300419, 0.295118, 0.318047, 0.283981, 0.301588, 0.307916, 0.296992,
      0.31139, 0.326333, 0.292356, 0.311593, 0.330921, 0.294845, 0.291656,
      0.291358, 0.307076, 0.299925, 0.314554, 0.290137, 0.286041, 0.318239,
      0.284017, 0.3183, 0.297905, 0.290357, 0.311856, 0.315954, 0.29857,
      0.340144, 0.3197, 0.297708, 0.282318, 0.312887, 0.28128, 0.286599,
      0.287291, 0.121044, 0.140354, 0.100358, 0.154209, 0.107667, 0.130407,
      0.120339, 0.0994624, 0.114788, 0.119922, 0.118756, 0.110087, 0.138795,
      0.112665, 0.132142, 0.137616, 0.122097, 0.134232, 0.126337, 0.110827,
      0.138686, 0.1051, 0.141232, 0.124776, 0.119412, 0.128429, 0.128451,
      0.108575, 0.110716, 0.14353, 0.109829, 0.121359, 0.110614, 0.132963,
      0.124317, 0.117972, 0.130076, 0.120891, 0.122477, 0.116104, 0.12113,
      0.117147, 0.115956, 0.104853, 0.11757, 0.121511, 0.105845, 0.0980316,
      0.130492, 0.120391, 0.135912, 0.109108, 0.12193, 0.112441, 0.10652,
      0.133285, 0.133186, 0.109064, 0.108575, 0.107551, 0.122689, 0.11835,
      0.125101, 0.123671, 0.151993, 0.115922, 0.119103, 0.102821, 0.119676,
      0.114857, 0.139638, 0.120894, 0.113278, 0.11386, 0.129762, 0.0912242,
      0.112099, 0.116769, 0.0995547, 0.120561, 0.105926, 0.0986941, 0.116581,
      0.0867449, 0.104724, 0.125425, 0.13433, 0.118297, 0.120495, 0.123478,
      0.145772, 0.121179, 0.110859, 0.130517, 0.108651, 0.0998347, 0.111099,
      0.110359, 0.125004, 0.121959, 1.98438, 2.00774, 2.01087, 1.97923, 2.00895,
      1.99588, 1.98338, 1.98529, 1.99956, 1.98229, 1.97317, 2.00243, 1.98959,
      2.00994, 1.99095, 2.0146, 1.97299, 2.02904, 2.00467, 2.01349, 2.00566,
      1.99303, 2.01064, 1.9857, 2.00713, 2.0029, 2.00333, 1.97494, 2.02467,
      2.00244, 2.00248, 2.00237, 2.0036, 1.99688, 2.01072, 1.97706, 1.99626,
      2.01956, 1.98919, 1.99658, 1.98548, 1.99389, 1.99099, 2.01419, 1.98741,
      1.9992, 2.01253, 1.97326, 2.02314, 2.02209, 2.01068, 2.00095, 1.98871,
      1.98088, 2.00173, 2.02792, 1.9815, 1.96628, 2.00007, 2.00602, 1.9771,
      2.02938, 1.99473, 2.02632, 2.00392, 2.0045, 2.01311, 1.99638, 1.96017,
      2.00244, 2.02357, 1.99722, 1.99866, 1.98423, 2.02426, 2.02849, 2.01944,
      1.97902, 1.98533, 1.99825, 2.02015, 1.96381, 2.01489, 1.99908, 1.99753,
      2.0119, 2.00604, 1.99721, 1.95694, 1.98723, 2.0011, 2.0165, 1.98348,
      2.03174, 2.00432, 1.99619, 2.00521, 2.00344, 2.00573, 2.0135, 1.42187,
      1.41029, 1.40841, 1.41786, 1.40757, 1.42118, 1.41653, 1.4139, 1.41187,
      1.42396, 1.42283, 1.41511, 1.41429, 1.41039, 1.41248, 1.40749, 1.41873,
      1.40217, 1.41333, 1.41572, 1.40811, 1.41546, 1.41305, 1.42353, 1.4138,
      1.41576, 1.41792, 1.42694, 1.40385, 1.41201, 1.41642, 1.41135, 1.41666,
      1.41379, 1.40889, 1.42425, 1.41904, 1.40642, 1.41645, 1.41383, 1.41897,
      1.41126, 1.42163, 1.40484, 1.41812, 1.41417, 1.41284, 1.42291, 1.39628,
      1.40186, 1.41274, 1.41554, 1.42209, 1.42161, 1.41781, 1.39898, 1.4277,
      1.43233, 1.41509, 1.41536, 1.43188, 1.40397, 1.41543, 1.40348, 1.41487,
      1.41148, 1.40937, 1.41626, 1.43505, 1.41034, 1.4062, 1.40617, 1.41283,
      1.42056, 1.40284, 1.4011, 1.4054, 1.41832, 1.41981, 1.41881, 1.40369,
      1.4316, 1.41009, 1.4148, 1.41229, 1.40394, 1.40857, 1.41509, 1.42688,
      1.41766, 1.41457, 1.40527, 1.42046, 1.40287, 1.41094, 1.42126, 1.41111,
      1.41128, 1.4131, 1.41286, 1.97844, 2.0019, 2.00447, 1.97323, 2.00279,
      1.99028, 1.9775, 1.97908, 1.99345, 1.97606, 1.96735, 1.99672, 1.984,
      2.00391, 1.98483, 2.00835, 1.96708, 2.02335, 1.99839, 2.00709, 1.99966,
      1.9869, 2.00468, 1.9797, 2.0012, 1.99735, 1.99755, 1.96884, 2.01856,
      1.99672, 1.99638, 1.99612, 1.99766, 1.99076, 2.00483, 1.97094, 1.99013,
      2.01345, 1.98323, 1.99044, 1.97946, 1.98774, 1.98526, 2.00837, 1.98091,
      1.99293, 2.00636, 1.9674, 2.01731, 2.01625, 2.00463, 1.99488, 1.98282,
      1.97502, 1.99562, 2.02198, 1.97528, 1.96019, 1.99405, 1.99997, 1.97101,
      2.02334, 1.98863, 2.0199, 1.99808, 1.99882, 2.00673, 1.99081, 1.95415,
      1.99613, 2.01805, 1.99122, 1.99246, 1.97832, 2.01812, 2.02189, 2.01349,
      1.97294, 1.97927, 1.99218, 2.01423, 1.95761, 2.00884, 1.99325, 1.99126,
      2.00628, 1.99983, 1.99124, 1.95123, 1.9812, 1.99534, 2.01039, 1.97687,
      2.02552, 1.99832, 1.99044, 1.99909, 1.9978, 1.99981, 2.0077, 29.6802,
      25.4113, 27.2614, 25.5535, 28.6859, 25.7313, 26.3175, 26.4423, 29.9833,
      27.5089, 22.6358, 29.3552, 26.0442, 28.1731, 27.3488, 27.0926, 25.7765,
      26.5589, 29.008, 24.7655, 28.5966, 28.861, 28.8555, 28.8517, 25.0678,
      27.7074, 26.6965, 28.3767, 24.1301, 28.6738, 23.6373, 29.8412, 28.3994,
      26.7635, 29.6554, 26.0291, 29.4057, 29.0149, 29.0988, 29.051, 29.5874,
      25.6407, 27.7069, 26.2523, 25.8804, 30.1078, 27.3133, 25.2109, 26.5539,
      29.0259, 28.4616, 29.225, 27.5692, 29.0283, 26.5378, 28.7928, 26.9245,
      27.8271, 28.9852, 28.6639, 24.2622, 28.7173, 30.3585, 26.0122, 27.8264,
      28.1662, 25.6457, 27.2283, 27.289, 29.3143, 25.4156, 23.9268, 28.0596,
      29.4136, 29.2869, 23.65, 28.8403, 24.8293, 27.9979, 25.41, 29.2564,
      26.0825, 26.9969, 25.328, 27.6732, 27.2414, 29.1196, 28.2973, 22.8898,
      29.9523, 26.7994, 29.6642, 25.1628, 26.1321, 29.0276, 26.5348, 30.1977,
      28.1052, 29.7211, 27.896, 17657.4, 17658.3, 17658.5, 17660.7, 17658.1,
      17660.8, 17660.2, 17660.1, 17656.5, 17659.7, 17659.2, 17657.2, 17659.9,
      17657.2, 17659.3, 17659, 17660.9, 17660.7, 17657.4, 17661.3, 17658.1,
      17657.6, 17657.5, 17658.3, 17660.6, 17658.4, 17659.2, 17659.3, 17662.6,
      17657.8, 17658.3, 17656.6, 17657.6, 17658.8, 17657.1, 17661.3, 17657,
      17658, 17656.3, 17657.4, 17656.8, 17659.3, 17659.1, 17660.3, 17659,
      17656.4, 17659.2, 17661.4, 17662.6, 17658.6, 17657.7, 17657, 17659.3,
      17657.6, 17660, 17659.6, 17660.4, 17661.2, 17657.5, 17657.9, 17663.6,
      17658.7, 17656, 17661.3, 17658.6, 17657.8, 17660.1, 17659, 17662.6,
      17657.3, 17661.2, 17660.9, 17657.4, 17657.2, 17658.2, 17664, 17658.3,
      17660.2, 17658.7, 17656.7, 17658.1, 17662.7, 17659.6, 17659.5, 17658.2,
      17660, 17657.6, 17655.7, 17664.5, 17656.8, 17659.5, 17657.5, 17661.1,
      17661.2, 17656.5, 17659.7, 17656.3, 17658.3, 17656.8, 17658.2;
  // std::cout << "\n\n" << prev_param_vals.format(CommaInitFmt) << "\n\n";
  Eigen::VectorXd prev_mean_vals = prev_param_vals.rowwise().mean().eval();
  //       std::cout << "Mean Values: \n" <<
  //       mean_vals.transpose().eval().format(CommaInitFmt) << "\n";
  Eigen::VectorXd prev_sd_vals = (((prev_param_vals.colwise() - prev_mean_vals)
                                       .array()
                                       .square()
                                       .matrix()
                                       .rowwise()
                                       .sum()
                                       .array()
                                   / (prev_param_vals.cols() - 1))
                                      .sqrt())
                                     .transpose()
                                     .eval();
  Eigen::MatrixXd ans_diff = param_vals - prev_param_vals;
  Eigen::VectorXd mean_diff_vals = ans_diff.rowwise().mean();
  //      std::cout << "diff Mean Values: \n" <<
  //      mean_diff_vals.transpose().eval().format(CommaInitFmt) << "\n";
  Eigen::VectorXd sd_diff_vals = (((ans_diff.colwise() - mean_diff_vals)
                                       .array()
                                       .square()
                                       .matrix()
                                       .rowwise()
                                       .sum()
                                       .array()
                                   / (ans_diff.cols() - 1))
                                      .sqrt())
                                     .transpose()
                                     .eval();

  Eigen::MatrixXd all_mean_vals(3, 10);
  all_mean_vals.row(0) = mean_vals;
  all_mean_vals.row(1) = prev_mean_vals;
  all_mean_vals.row(2) = mean_diff_vals;

  Eigen::MatrixXd all_sd_vals(3, 10);
  all_sd_vals.row(0) = sd_vals;
  all_sd_vals.row(1) = prev_sd_vals;
  all_sd_vals.row(2) = sd_diff_vals;
  for (int i = 0; i < all_mean_vals.cols() - 1; ++i) {
    EXPECT_NEAR(0, all_mean_vals(2, i), 1);
  }
  // EXPECT_NEAR(0, all_mean_vals(2, 9), 0.05);
  /*
  std::cout << "\nMean vals:\n" << all_mean_vals.format(CommaInitFmt) << "\n";
  std::cout << "\nSD vals:\n" << all_sd_vals.format(CommaInitFmt) << "\n";
  std::cout << "\nMean vals:\n" << mean_vals.format(CommaInitFmt) << "\n";
  std::cout << "\nSD vals:\n" << sd_vals.format(CommaInitFmt) << "\n";
  */
}

TEST_F(ServicesPathfinderGLM, multi) {
  constexpr unsigned int seed = 0;
  constexpr unsigned int chain = 1;
  constexpr double init_radius = 1;
  constexpr double num_multi_draws = 100;
  constexpr int num_paths = 4;
  constexpr double num_elbo_draws = 1000;
  constexpr double num_draws = 2000;
  constexpr int history_size = 15;
  constexpr double init_alpha = 1;
  constexpr double tol_obj = 0;
  constexpr double tol_rel_obj = 0;
  constexpr double tol_grad = 0;
  constexpr double tol_rel_grad = 0;
  constexpr double tol_param = 0;
  constexpr int num_iterations = 220;
  constexpr bool save_iterations = false;
  constexpr int refresh = 0;
  constexpr int num_eval_attempts = 10;

  std::ostream empty_ostream(nullptr);
  stan::test::loggy logger(empty_ostream);
  std::vector<stan::callbacks::writer> single_path_parameter_writer(num_paths);
  std::vector<stan::callbacks::writer> single_path_diagnostic_writer(num_paths);
  std::vector<std::unique_ptr<decltype(init_init_context())>> single_path_inits;
  for (int i = 0; i < num_paths; ++i) {
    //    single_path_parameter_writer.emplace_back(single_path_parameter_ss[i]);
    //    single_path_diagnostic_writer.emplace_back(single_path_diagnostic_ss[i]);
    single_path_inits.emplace_back(
        std::make_unique<decltype(init_init_context())>(init_init_context()));
  }
  // int refresh = 0;
  stan::test::mock_callback callback;
  int return_code = stan::services::pathfinder::pathfinder_lbfgs_multi(
      model, single_path_inits, seed, chain, init_radius, history_size,
      init_alpha, tol_obj, tol_rel_obj, tol_grad, tol_rel_grad, tol_param,
      num_iterations, save_iterations, refresh, callback, num_elbo_draws,
      num_draws, num_multi_draws, num_eval_attempts, num_paths, logger,
      std::vector<stan::callbacks::stream_writer>(num_paths, init),
      single_path_parameter_writer, single_path_diagnostic_writer, parameter,
      diagnostics);

  // Eigen::MatrixXd param_vals = parameter.values_.transpose();
  // Eigen::MatrixXd param_vals = parameter.values_.transpose();
  Eigen::MatrixXd param_vals(parameter.eigen_states_.size(),
                             parameter.eigen_states_[0].size());
  for (size_t i = 0; i < parameter.eigen_states_.size(); ++i) {
    param_vals.row(i) = parameter.eigen_states_[i];
  }
  param_vals.transposeInPlace();
  // std::cout << "\n --- Optim Path ---" << std::endl;
  for (Eigen::Index i = 0; i < diagnostics.optim_path_.size(); ++i) {
    Eigen::MatrixXd tmp(2, param_vals.cols() - 1);
    tmp.row(0) = std::get<0>(diagnostics.optim_path_[i]);
    tmp.row(1) = std::get<1>(diagnostics.optim_path_[i]);
    // std::cout << "Iter: " << i << "\n" << tmp << "\n";
  }
  Eigen::IOFormat CommaInitFmt(Eigen::StreamPrecision, 0, ", ", ", ", "\n", "",
                               "", "");

  // std::cout << "---- Results  -------" << std::endl;
  Eigen::VectorXd mean_vals = param_vals.rowwise().mean().eval();
  //       std::cout << "Mean Values: \n" <<
  //       mean_vals.transpose().eval().format(CommaInitFmt) << "\n";
  Eigen::VectorXd sd_vals = (((param_vals.colwise() - mean_vals)
                                  .array()
                                  .square()
                                  .matrix()
                                  .rowwise()
                                  .sum()
                                  .array()
                              / (param_vals.cols() - 1))
                                 .sqrt())
                                .transpose()
                                .eval();
  // std::cout << "\n\n" << param_vals.format(CommaInitFmt) << "\n\n";
  // std::cout << "rows: " << param_vals.rows() << " cols: " <<
  // param_vals.cols() << "\n";
  Eigen::MatrixXd prev_param_vals(10, 100);
  prev_param_vals << 0.989133, 0.987283, 1.00865, 1.01761, 1.02141, 0.979206,
      1.0016, 1.02329, 1.01547, 1.00124, 1.00566, 0.982438, 1.01388, 0.990802,
      1.02231, 1.00406, 1.01691, 0.997928, 0.997293, 0.974797, 1.00595, 1.01402,
      0.984706, 0.977346, 1.00434, 0.974, 0.963947, 0.999307, 0.98874, 0.997258,
      0.992295, 1.01447, 0.989742, 1.00031, 1.00598, 0.997586, 0.989764,
      1.00578, 1.00274, 1.00065, 1.00799, 1.01237, 0.96816, 1.0062, 1.01169,
      1.00744, 0.980485, 1.00712, 1.02736, 1.00107, 0.986573, 1.00091, 0.998115,
      0.99281, 0.995292, 1.00851, 0.984376, 0.98788, 1.00403, 0.976502,
      0.982548, 0.992574, 1.00651, 1.01491, 0.985712, 0.990779, 1.00292,
      0.984381, 0.985727, 1.02072, 0.972444, 1.02651, 1.00152, 1.00147, 1.00619,
      1.0108, 1.00412, 1.02858, 1.00949, 0.985685, 1.00205, 0.990218, 0.997355,
      0.994934, 1.01341, 1.01248, 1.01124, 1.00118, 1.01047, 1.00284, 0.978697,
      1.01447, 1.00973, 0.990684, 1.00624, 0.976974, 1.00119, 0.996196,
      0.999012, 0.983142, 0.68016, 0.699344, 0.673045, 0.691876, 0.669649,
      0.672762, 0.716136, 0.697584, 0.674966, 0.650528, 0.696122, 0.687595,
      0.690818, 0.69065, 0.680874, 0.667614, 0.703743, 0.68247, 0.674234,
      0.656675, 0.69866, 0.667602, 0.671322, 0.682141, 0.65578, 0.691154,
      0.690077, 0.661168, 0.682424, 0.686199, 0.669013, 0.671651, 0.662703,
      0.681059, 0.6717, 0.667242, 0.662769, 0.661001, 0.691038, 0.693961,
      0.67983, 0.697614, 0.695461, 0.688362, 0.666684, 0.669772, 0.678979,
      0.695811, 0.701869, 0.699734, 0.666627, 0.663433, 0.657691, 0.693838,
      0.656416, 0.691867, 0.652502, 0.66561, 0.661675, 0.681262, 0.639635,
      0.672269, 0.676344, 0.654824, 0.680483, 0.693304, 0.673858, 0.69701,
      0.669327, 0.670104, 0.700511, 0.712719, 0.68641, 0.676175, 0.673863,
      0.671275, 0.671586, 0.670274, 0.667039, 0.665298, 0.6905, 0.678944,
      0.672464, 0.685418, 0.675042, 0.698088, 0.686032, 0.677391, 0.712156,
      0.68469, 0.697393, 0.669891, 0.663002, 0.677229, 0.677726, 0.668302,
      0.679998, 0.699712, 0.671381, 0.663888, 0.511369, 0.525085, 0.531038,
      0.529463, 0.529189, 0.543642, 0.535529, 0.533005, 0.527877, 0.509799,
      0.536219, 0.519464, 0.513025, 0.540542, 0.502001, 0.511964, 0.507174,
      0.535565, 0.54626, 0.54512, 0.533558, 0.516238, 0.529573, 0.525098,
      0.544148, 0.512651, 0.532813, 0.525611, 0.49472, 0.50992, 0.553442,
      0.536401, 0.532948, 0.502132, 0.513881, 0.492237, 0.524224, 0.517362,
      0.523663, 0.542269, 0.537562, 0.509463, 0.524809, 0.493832, 0.525565,
      0.537415, 0.550462, 0.543065, 0.516061, 0.522298, 0.515901, 0.532414,
      0.532221, 0.523204, 0.552887, 0.526091, 0.530968, 0.530605, 0.51651,
      0.528895, 0.532424, 0.536296, 0.532983, 0.548964, 0.535942, 0.504232,
      0.563204, 0.515654, 0.536047, 0.53497, 0.517214, 0.525378, 0.518352,
      0.516509, 0.522678, 0.543605, 0.508595, 0.511166, 0.517616, 0.526743,
      0.522237, 0.54086, 0.560177, 0.52307, 0.509021, 0.504419, 0.538517,
      0.524971, 0.533303, 0.514497, 0.515468, 0.522429, 0.523632, 0.5504,
      0.516653, 0.520695, 0.520476, 0.531572, 0.537675, 0.514285, 0.30942,
      0.314056, 0.322901, 0.307963, 0.29414, 0.268925, 0.312988, 0.314239,
      0.298855, 0.308249, 0.290594, 0.291957, 0.275043, 0.311282, 0.313345,
      0.32358, 0.309169, 0.277428, 0.320151, 0.327048, 0.314795, 0.297986,
      0.305475, 0.318588, 0.270818, 0.29099, 0.309577, 0.296313, 0.330638,
      0.295411, 0.295335, 0.308153, 0.286721, 0.326388, 0.289772, 0.317589,
      0.311338, 0.300116, 0.306695, 0.317983, 0.296273, 0.32718, 0.305991,
      0.297433, 0.332018, 0.311691, 0.315756, 0.285922, 0.293896, 0.303194,
      0.313389, 0.292248, 0.276507, 0.303205, 0.285911, 0.30499, 0.313411,
      0.302989, 0.290302, 0.317428, 0.290353, 0.301325, 0.304949, 0.307035,
      0.300419, 0.295118, 0.318047, 0.283981, 0.301588, 0.307916, 0.296992,
      0.31139, 0.326333, 0.292356, 0.311593, 0.330921, 0.294845, 0.291656,
      0.291358, 0.307076, 0.299925, 0.314554, 0.290137, 0.286041, 0.318239,
      0.284017, 0.3183, 0.297905, 0.290357, 0.311856, 0.315954, 0.29857,
      0.340144, 0.3197, 0.297708, 0.282318, 0.312887, 0.28128, 0.286599,
      0.287291, 0.121044, 0.140354, 0.100358, 0.154209, 0.107667, 0.130407,
      0.120339, 0.0994624, 0.114788, 0.119922, 0.118756, 0.110087, 0.138795,
      0.112665, 0.132142, 0.137616, 0.122097, 0.134232, 0.126337, 0.110827,
      0.138686, 0.1051, 0.141232, 0.124776, 0.119412, 0.128429, 0.128451,
      0.108575, 0.110716, 0.14353, 0.109829, 0.121359, 0.110614, 0.132963,
      0.124317, 0.117972, 0.130076, 0.120891, 0.122477, 0.116104, 0.12113,
      0.117147, 0.115956, 0.104853, 0.11757, 0.121511, 0.105845, 0.0980316,
      0.130492, 0.120391, 0.135912, 0.109108, 0.12193, 0.112441, 0.10652,
      0.133285, 0.133186, 0.109064, 0.108575, 0.107551, 0.122689, 0.11835,
      0.125101, 0.123671, 0.151993, 0.115922, 0.119103, 0.102821, 0.119676,
      0.114857, 0.139638, 0.120894, 0.113278, 0.11386, 0.129762, 0.0912242,
      0.112099, 0.116769, 0.0995547, 0.120561, 0.105926, 0.0986941, 0.116581,
      0.0867449, 0.104724, 0.125425, 0.13433, 0.118297, 0.120495, 0.123478,
      0.145772, 0.121179, 0.110859, 0.130517, 0.108651, 0.0998347, 0.111099,
      0.110359, 0.125004, 0.121959, 1.98438, 2.00774, 2.01087, 1.97923, 2.00895,
      1.99588, 1.98338, 1.98529, 1.99956, 1.98229, 1.97317, 2.00243, 1.98959,
      2.00994, 1.99095, 2.0146, 1.97299, 2.02904, 2.00467, 2.01349, 2.00566,
      1.99303, 2.01064, 1.9857, 2.00713, 2.0029, 2.00333, 1.97494, 2.02467,
      2.00244, 2.00248, 2.00237, 2.0036, 1.99688, 2.01072, 1.97706, 1.99626,
      2.01956, 1.98919, 1.99658, 1.98548, 1.99389, 1.99099, 2.01419, 1.98741,
      1.9992, 2.01253, 1.97326, 2.02314, 2.02209, 2.01068, 2.00095, 1.98871,
      1.98088, 2.00173, 2.02792, 1.9815, 1.96628, 2.00007, 2.00602, 1.9771,
      2.02938, 1.99473, 2.02632, 2.00392, 2.0045, 2.01311, 1.99638, 1.96017,
      2.00244, 2.02357, 1.99722, 1.99866, 1.98423, 2.02426, 2.02849, 2.01944,
      1.97902, 1.98533, 1.99825, 2.02015, 1.96381, 2.01489, 1.99908, 1.99753,
      2.0119, 2.00604, 1.99721, 1.95694, 1.98723, 2.0011, 2.0165, 1.98348,
      2.03174, 2.00432, 1.99619, 2.00521, 2.00344, 2.00573, 2.0135, 1.42187,
      1.41029, 1.40841, 1.41786, 1.40757, 1.42118, 1.41653, 1.4139, 1.41187,
      1.42396, 1.42283, 1.41511, 1.41429, 1.41039, 1.41248, 1.40749, 1.41873,
      1.40217, 1.41333, 1.41572, 1.40811, 1.41546, 1.41305, 1.42353, 1.4138,
      1.41576, 1.41792, 1.42694, 1.40385, 1.41201, 1.41642, 1.41135, 1.41666,
      1.41379, 1.40889, 1.42425, 1.41904, 1.40642, 1.41645, 1.41383, 1.41897,
      1.41126, 1.42163, 1.40484, 1.41812, 1.41417, 1.41284, 1.42291, 1.39628,
      1.40186, 1.41274, 1.41554, 1.42209, 1.42161, 1.41781, 1.39898, 1.4277,
      1.43233, 1.41509, 1.41536, 1.43188, 1.40397, 1.41543, 1.40348, 1.41487,
      1.41148, 1.40937, 1.41626, 1.43505, 1.41034, 1.4062, 1.40617, 1.41283,
      1.42056, 1.40284, 1.4011, 1.4054, 1.41832, 1.41981, 1.41881, 1.40369,
      1.4316, 1.41009, 1.4148, 1.41229, 1.40394, 1.40857, 1.41509, 1.42688,
      1.41766, 1.41457, 1.40527, 1.42046, 1.40287, 1.41094, 1.42126, 1.41111,
      1.41128, 1.4131, 1.41286, 1.97844, 2.0019, 2.00447, 1.97323, 2.00279,
      1.99028, 1.9775, 1.97908, 1.99345, 1.97606, 1.96735, 1.99672, 1.984,
      2.00391, 1.98483, 2.00835, 1.96708, 2.02335, 1.99839, 2.00709, 1.99966,
      1.9869, 2.00468, 1.9797, 2.0012, 1.99735, 1.99755, 1.96884, 2.01856,
      1.99672, 1.99638, 1.99612, 1.99766, 1.99076, 2.00483, 1.97094, 1.99013,
      2.01345, 1.98323, 1.99044, 1.97946, 1.98774, 1.98526, 2.00837, 1.98091,
      1.99293, 2.00636, 1.9674, 2.01731, 2.01625, 2.00463, 1.99488, 1.98282,
      1.97502, 1.99562, 2.02198, 1.97528, 1.96019, 1.99405, 1.99997, 1.97101,
      2.02334, 1.98863, 2.0199, 1.99808, 1.99882, 2.00673, 1.99081, 1.95415,
      1.99613, 2.01805, 1.99122, 1.99246, 1.97832, 2.01812, 2.02189, 2.01349,
      1.97294, 1.97927, 1.99218, 2.01423, 1.95761, 2.00884, 1.99325, 1.99126,
      2.00628, 1.99983, 1.99124, 1.95123, 1.9812, 1.99534, 2.01039, 1.97687,
      2.02552, 1.99832, 1.99044, 1.99909, 1.9978, 1.99981, 2.0077, 29.6802,
      25.4113, 27.2614, 25.5535, 28.6859, 25.7313, 26.3175, 26.4423, 29.9833,
      27.5089, 22.6358, 29.3552, 26.0442, 28.1731, 27.3488, 27.0926, 25.7765,
      26.5589, 29.008, 24.7655, 28.5966, 28.861, 28.8555, 28.8517, 25.0678,
      27.7074, 26.6965, 28.3767, 24.1301, 28.6738, 23.6373, 29.8412, 28.3994,
      26.7635, 29.6554, 26.0291, 29.4057, 29.0149, 29.0988, 29.051, 29.5874,
      25.6407, 27.7069, 26.2523, 25.8804, 30.1078, 27.3133, 25.2109, 26.5539,
      29.0259, 28.4616, 29.225, 27.5692, 29.0283, 26.5378, 28.7928, 26.9245,
      27.8271, 28.9852, 28.6639, 24.2622, 28.7173, 30.3585, 26.0122, 27.8264,
      28.1662, 25.6457, 27.2283, 27.289, 29.3143, 25.4156, 23.9268, 28.0596,
      29.4136, 29.2869, 23.65, 28.8403, 24.8293, 27.9979, 25.41, 29.2564,
      26.0825, 26.9969, 25.328, 27.6732, 27.2414, 29.1196, 28.2973, 22.8898,
      29.9523, 26.7994, 29.6642, 25.1628, 26.1321, 29.0276, 26.5348, 30.1977,
      28.1052, 29.7211, 27.896, 17657.4, 17658.3, 17658.5, 17660.7, 17658.1,
      17660.8, 17660.2, 17660.1, 17656.5, 17659.7, 17659.2, 17657.2, 17659.9,
      17657.2, 17659.3, 17659, 17660.9, 17660.7, 17657.4, 17661.3, 17658.1,
      17657.6, 17657.5, 17658.3, 17660.6, 17658.4, 17659.2, 17659.3, 17662.6,
      17657.8, 17658.3, 17656.6, 17657.6, 17658.8, 17657.1, 17661.3, 17657,
      17658, 17656.3, 17657.4, 17656.8, 17659.3, 17659.1, 17660.3, 17659,
      17656.4, 17659.2, 17661.4, 17662.6, 17658.6, 17657.7, 17657, 17659.3,
      17657.6, 17660, 17659.6, 17660.4, 17661.2, 17657.5, 17657.9, 17663.6,
      17658.7, 17656, 17661.3, 17658.6, 17657.8, 17660.1, 17659, 17662.6,
      17657.3, 17661.2, 17660.9, 17657.4, 17657.2, 17658.2, 17664, 17658.3,
      17660.2, 17658.7, 17656.7, 17658.1, 17662.7, 17659.6, 17659.5, 17658.2,
      17660, 17657.6, 17655.7, 17664.5, 17656.8, 17659.5, 17657.5, 17661.1,
      17661.2, 17656.5, 17659.7, 17656.3, 17658.3, 17656.8, 17658.2;
  // std::cout << "\n\n" << prev_param_vals.format(CommaInitFmt) << "\n\n";
  Eigen::VectorXd prev_mean_vals = prev_param_vals.rowwise().mean().eval();
  //       std::cout << "Mean Values: \n" <<
  //       mean_vals.transpose().eval().format(CommaInitFmt) << "\n";
  Eigen::VectorXd prev_sd_vals = (((prev_param_vals.colwise() - prev_mean_vals)
                                       .array()
                                       .square()
                                       .matrix()
                                       .rowwise()
                                       .sum()
                                       .array()
                                   / (prev_param_vals.cols() - 1))
                                      .sqrt())
                                     .transpose()
                                     .eval();
  Eigen::MatrixXd ans_diff = param_vals - prev_param_vals;
  Eigen::VectorXd mean_diff_vals = ans_diff.rowwise().mean();
  //      std::cout << "diff Mean Values: \n" <<
  //      mean_diff_vals.transpose().eval().format(CommaInitFmt) << "\n";
  Eigen::VectorXd sd_diff_vals = (((ans_diff.colwise() - mean_diff_vals)
                                       .array()
                                       .square()
                                       .matrix()
                                       .rowwise()
                                       .sum()
                                       .array()
                                   / (ans_diff.cols() - 1))
                                      .sqrt())
                                     .transpose()
                                     .eval();

  Eigen::MatrixXd all_mean_vals(3, 10);
  all_mean_vals.row(0) = mean_vals;
  all_mean_vals.row(1) = prev_mean_vals;
  all_mean_vals.row(2) = mean_diff_vals;

  Eigen::MatrixXd all_sd_vals(3, 10);
  all_sd_vals.row(0) = sd_vals;
  all_sd_vals.row(1) = prev_sd_vals;
  all_sd_vals.row(2) = sd_diff_vals;
  for (int i = 0; i < all_mean_vals.cols() - 2; ++i) {
    EXPECT_NEAR(0, all_mean_vals(2, i), 1);
  }
  // EXPECT_NEAR(0, all_mean_vals(2, 9), 10);
  /*
  std::cout << "\nMean vals:\n" << all_mean_vals.format(CommaInitFmt) << "\n";
  std::cout << "\nSD vals:\n" << all_sd_vals.format(CommaInitFmt) << "\n";

  std::cout << "\nMean vals:\n" << mean_vals.format(CommaInitFmt) << "\n";
  std::cout << "\nSD vals:\n" << sd_vals.format(CommaInitFmt) << "\n";
  */
}
