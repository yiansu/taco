// Generated by the Tensor Algebra Compiler (tensor-compiler.org)
// taco "y(i) = A(i, j) * x(j)" -f=y:d -f=A:ds -f=x:d -write-assembly=assemble.hpp -write-compute=compute.hpp
// clang++ -std=c++17 -O3 -DNDEBUG -DTACO -I ../../include -L../../build/lib SPMV.cpp -o SPMV -ltaco

#include <random>
#include "taco.h"
#include "assemble.hpp"
#include "compute.hpp"

using namespace taco;
int main(int argc, char* argv[]) {
  std::default_random_engine gen(0);
  std::uniform_real_distribution<double> unif(0.0, 1.0);

  Format ds({Dense, Sparse});
  Format d({Dense});

  std::string matrix = "pwtk";
  if(const char* env_matrix = std::getenv("MATRIX")) {
    matrix = env_matrix;
  }
  std::cout << "matrix = " << matrix << std::endl;
  Tensor<double> A = read("matrices/" + matrix + ".mtx", ds);

  Tensor<double> x({A.getDimension(1)}, d);
  for (int i = 0; i < x.getDimension(0); ++i) {
    x.insert({i}, unif(gen));
  }
  x.pack();

  Tensor<double> y({A.getDimension(0)}, d);
  y.pack();

  // assemble(y.getTacoTensorT(), A.getTacoTensorT(), x.getTacoTensorT());
  compute(y.getTacoTensorT(), A.getTacoTensorT(), x.getTacoTensorT());

  return 0;
}
