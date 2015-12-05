#include <gtest/gtest.h>
#include <stan/model/new_array.hpp>
#include <vector>
#include <test/unit/util.hpp>

TEST(ModelNewArray, intArray) {
  std::vector<int> x;
  x.push_back(1);
  x.push_back(2);

  std::vector<int> z = stan::model::new_array<int>().add(1).add(2).array();

  EXPECT_EQ(2, z.size());
  EXPECT_EQ(x[0], z[0]);
  EXPECT_EQ(x[1], z[1]);
}

TEST(ModelNewArray, doubleArray) {
  std::vector<double> x;
  x.push_back(1.1);
  x.push_back(2.2);

  stan::model::new_array<double> y;
  y.add(1.1).add(2.2);

  std::vector<double> z = y.array();
  EXPECT_EQ(2, z.size());
  EXPECT_FLOAT_EQ(x[0], z[0]);
  EXPECT_FLOAT_EQ(x[1], z[1]);
}

TEST(ModelNewArray, doubleArrayArray) {
  std::vector<std::vector<double> > x;
  std::vector<double> x1;
  std::vector<double> x2;
  x1.push_back(1.1);
  x1.push_back(2.2);
  x2.push_back(1.1);
  x2.push_back(2.2);
  x.push_back(x1);
  x.push_back(x2);
  
  stan::model::new_array<stan::model::new_array<double> > y;
  stan::model::new_array<double> y1;
  y1.add(1.1).add(2.2);
  stan::model::new_array<double> y2;
  y2.add(1.1).add(2.2);
  y.add(y1).add(y2);
  
  std::vector<stan::model::new_array<double> > z = y.array();
  EXPECT_EQ(2, z.size());
}

TEST(ModelNewArray, chainDoubleA2) {

  std::vector<stan::model::new_array<int> > z = 
    stan::model::new_array<stan::model::new_array<int> >().add(stan::model::new_array<int>().add(1).add(2)).add(stan::model::new_array<int>().add(1).add(2)).array();

  EXPECT_EQ(2, z.size());

  std::vector<int> z1 = z[0].array();
  std::vector<int> z2 = z[1].array();

  std::vector<int> x;
  x.push_back(1);
  x.push_back(2);
  
  EXPECT_EQ(x[0], z1[0]);
  EXPECT_EQ(x[1], z1[1]);

  EXPECT_EQ(x[0], z2[0]);
  EXPECT_EQ(x[1], z2[1]);
}
