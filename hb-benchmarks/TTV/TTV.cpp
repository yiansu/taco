// Generated by the Tensor Algebra Compiler (tensor-compiler.org)
// taco "A(i, j) = B(i, j, k) * c(k)" -f=A:dd -f=B:dss -f=c:d -write-assembly=assemble.hpp -write-compute=compute.hpp
// clang++ -std=c++17 -O3 -DNDEBUG -DTACO -I ../../include -L../../build/lib TTV.cpp -o main -ltaco

#include <random>
#include "taco.h"
#include "assemble.hpp"
#include "compute.hpp"

using namespace taco;
int main(int argc, char* argv[]) {
  std::default_random_engine gen(0);
  std::uniform_real_distribution<double> unif(0.0, 1.0);

  Format dd({Dense, Dense});
  Format dss({Dense, Sparse, Sparse});
  Format d({Dense});

  Tensor<double> B = read("nell-2.tns", dss);
  for (auto dim : B.getDimensions()) {
    printf("dim = %d\n", dim);
  }

  Tensor<double> c({B.getDimension(2)}, d);
  for (int i = 0; i < c.getDimension(0); ++i) {
    c.insert({i}, unif(gen));
  }
  c.pack();

  Tensor<double> A({B.getDimension(0), B.getDimension(1)}, dd);
  A.pack();

  // assemble(A.getTacoTensorT(), B.getTacoTensorT(), c.getTacoTensorT());
  compute(A.getTacoTensorT(), B.getTacoTensorT(), c.getTacoTensorT());

  return 0;
}
