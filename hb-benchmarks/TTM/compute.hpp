// Generated by the Tensor Algebra Compiler (tensor-compiler.org)
// taco "A(i, j, k) = B(i, j, l) * C(k, l)" -f=A:ddd -f=B:dss -f=C:dd -write-assembly=assemble.hpp -write-compute=compute.hpp

#if defined(USE_BASELINE) || defined(USE_FORKJOIN)
#include <functional>
#include <taskparts/benchmark.hpp>
#elif defined(USE_OPENMP)
#include "utility.hpp"
#else
#include "loop_handler.hpp"
#endif

#if defined(TEST_CORRECTNESS)
#include "taco.h"
using namespace taco;
#endif

#if defined(USE_BASELINE) || defined(TEST_CORRECTNESS)
void TTM_baseline(
  int A1_dimension,
  int A2_dimension,
  int A3_dimension,
  double* __restrict__ A_vals,
  int B1_dimension,
  int* __restrict__ B2_pos,
  int* __restrict__ B2_crd,
  int* __restrict__ B3_pos,
  int* __restrict__ B3_crd,
  double* __restrict__ B_vals,
  int C1_dimension,
  int C2_dimension,
  double* __restrict__ C_vals
) {
  for (uint64_t pA = 0; pA < ((A1_dimension * A2_dimension) * A3_dimension); pA++) {
    A_vals[pA] = 0.0;
  }

  for (uint64_t i = 0; i < B1_dimension; i++) {
    for (uint64_t jB = B2_pos[i]; jB < B2_pos[(i + 1)]; jB++) {
      uint64_t j = B2_crd[jB];
      uint64_t jA = i * A2_dimension + j;
      for (uint64_t k = 0; k < C1_dimension; k++) {
        uint64_t kA = jA * A3_dimension + k;
        double tlA_val = 0.0;
        for (uint64_t lB = B3_pos[jB]; lB < B3_pos[(jB + 1)]; lB++) {
          uint64_t l = B3_crd[lB];
          uint64_t lC = k * C2_dimension + l;
          tlA_val += B_vals[lB] * C_vals[lC];
        }
        A_vals[kA] = tlA_val;
      }
    }
  }
}

#if defined(TEST_CORRECTNESS)
void test_correctness(
  int A1_dimension,
  int A2_dimension,
  int A3_dimension,
  double* __restrict__ A_vals,
  int B1_dimension,
  int* __restrict__ B2_pos,
  int* __restrict__ B2_crd,
  int* __restrict__ B3_pos,
  int* __restrict__ B3_crd,
  double* __restrict__ B_vals,
  int C1_dimension,
  int C2_dimension,
  double* __restrict__ C_vals
) {
  double* __restrict__ A_vals_ref = (double*)malloc(sizeof(double) * ((A1_dimension * A2_dimension) * A3_dimension));

  TTM_baseline(A1_dimension, A2_dimension, A3_dimension, A_vals_ref, B1_dimension, B2_pos, B2_crd, B3_pos, B3_crd, B_vals, C1_dimension, C2_dimension, C_vals);

  uint64_t num_diffs = 0;
  double epsilon = 0.01;
  for (uint64_t i = 0; i < ((A1_dimension * A2_dimension) * A3_dimension); i++) {
    auto diff = std::abs(A_vals[i] - A_vals_ref[i]);
    if (diff > epsilon) {
      printf("diff=%f A_vals[i]=%f A_vals_ref[i]=%f at i=%lu\n", diff, A_vals[i], A_vals_ref[i], i);
      num_diffs++;
    }
  }
  if (num_diffs > 0) {
    printf("\033[0;31mINCORRECT!\033[0m");
    printf("  num_diffs = %lu\n", num_diffs);
  } else {
    printf("\033[0;32mCORRECT!\033[0m\n");
  }
}

#endif
#endif

#if defined(USE_OPENMP)

#include <omp.h>

void TTM_openmp(
  int A1_dimension,
  int A2_dimension,
  int A3_dimension,
  double* __restrict__ A_vals,
  int B1_dimension,
  int* __restrict__ B2_pos,
  int* __restrict__ B2_crd,
  int* __restrict__ B3_pos,
  int* __restrict__ B3_crd,
  double* __restrict__ B_vals,
  int C1_dimension,
  int C2_dimension,
  double* __restrict__ C_vals
) {
#if defined(OMP_NESTED_PARALLELISM)
  omp_set_max_active_levels(4);
#endif
  #pragma omp parallel for schedule(static)
  for (uint64_t pA = 0; pA < ((A1_dimension * A2_dimension) * A3_dimension); pA++) {
    A_vals[pA] = 0.0;
  }

#if !defined(OMP_CHUNKSIZE)
#if defined(OMP_SCHEDULE_STATIC)
  #pragma omp parallel for schedule(static)
#elif defined(OMP_SCHEDULE_DYNAMIC)
  #pragma omp parallel for schedule(dynamic)
#elif defined(OMP_SCHEDULE_GUIDED)
  #pragma omp parallel for schedule(guided)
#endif
#else
#if defined(OMP_SCHEDULE_STATIC)
  #pragma omp parallel for schedule(static, OMP_CHUNKSIZE)
#elif defined(OMP_SCHEDULE_DYNAMIC)
  #pragma omp parallel for schedule(dynamic, OMP_CHUNKSIZE)
#elif defined(OMP_SCHEDULE_GUIDED)
  #pragma omp parallel for schedule(guided, OMP_CHUNKSIZE)
#endif
#endif
  for (uint64_t i = 0; i < B1_dimension; i++) {
#if defined(OMP_NESTED_PARALLELISM)
#if !defined(OMP_CHUNKSIZE)
#if defined(OMP_SCHEDULE_STATIC)
    #pragma omp parallel for schedule(static)
#elif defined(OMP_SCHEDULE_DYNAMIC)
    #pragma omp parallel for schedule(dynamic)
#elif defined(OMP_SCHEDULE_GUIDED)
    #pragma omp parallel for schedule(guided)
#endif
#else
#if defined(OMP_SCHEDULE_STATIC)
    #pragma omp parallel for schedule(static, OMP_CHUNKSIZE)
#elif defined(OMP_SCHEDULE_DYNAMIC)
    #pragma omp parallel for schedule(dynamic, OMP_CHUNKSIZE)
#elif defined(OMP_SCHEDULE_GUIDED)
    #pragma omp parallel for schedule(guided, OMP_CHUNKSIZE)
#endif
#endif
#endif
    for (uint64_t jB = B2_pos[i]; jB < B2_pos[(i + 1)]; jB++) {
      uint64_t j = B2_crd[jB];
      uint64_t jA = i * A2_dimension + j;
#if defined(OMP_NESTED_PARALLELISM)
#if !defined(OMP_CHUNKSIZE)
#if defined(OMP_SCHEDULE_STATIC)
      #pragma omp parallel for schedule(static)
#elif defined(OMP_SCHEDULE_DYNAMIC)
      #pragma omp parallel for schedule(dynamic)
#elif defined(OMP_SCHEDULE_GUIDED)
      #pragma omp parallel for schedule(guided)
#endif
#else
#if defined(OMP_SCHEDULE_STATIC)
      #pragma omp parallel for schedule(static, OMP_CHUNKSIZE)
#elif defined(OMP_SCHEDULE_DYNAMIC)
      #pragma omp parallel for schedule(dynamic, OMP_CHUNKSIZE)
#elif defined(OMP_SCHEDULE_GUIDED)
      #pragma omp parallel for schedule(guided, OMP_CHUNKSIZE)
#endif
#endif
#endif
      for (uint64_t k = 0; k < C1_dimension; k++) {
        uint64_t kA = jA * A3_dimension + k;
        double tlA_val = 0.0;
#if defined(OMP_NESTED_PARALLELISM)
#if !defined(OMP_CHUNKSIZE)
#if defined(OMP_SCHEDULE_STATIC)
        #pragma omp parallel for schedule(static) reduction(+:tlA_val)
#elif defined(OMP_SCHEDULE_DYNAMIC)
        #pragma omp parallel for schedule(dynamic) reduction(+:tlA_val)
#elif defined(OMP_SCHEDULE_GUIDED)
        #pragma omp parallel for schedule(guided) reduction(+:tlA_val)
#endif
#else
#if defined(OMP_SCHEDULE_STATIC)
        #pragma omp parallel for schedule(static, OMP_CHUNKSIZE) reduction(+:tlA_val)
#elif defined(OMP_SCHEDULE_DYNAMIC)
        #pragma omp parallel for schedule(dynamic, OMP_CHUNKSIZE) reduction(+:tlA_val)
#elif defined(OMP_SCHEDULE_GUIDED)
        #pragma omp parallel for schedule(guided, OMP_CHUNKSIZE) reduction(+:tlA_val)
#endif
#endif
#endif
        for (uint64_t lB = B3_pos[jB]; lB < B3_pos[(jB + 1)]; lB++) {
          uint64_t l = B3_crd[lB];
          uint64_t lC = k * C2_dimension + l;
          tlA_val += B_vals[lB] * C_vals[lC];
        }
        A_vals[kA] = tlA_val;
      }
    }
  }
}

#elif defined(USE_HB_COMPILER)
#if defined(RUN_HEARTBEAT)
  bool run_heartbeat = true;
#else
  bool run_heartbeat = false;
#endif

void HEARTBEAT_loop0(
  uint64_t maxIter,
  double* __restrict__ A_vals
) {
  for (uint64_t pA = 0; pA < maxIter; pA++) {
    A_vals[pA] = 0.0;
  }
}

double HEARTBEAT_loop4(
  uint64_t startIter,
  uint64_t maxIter,
  int* __restrict__ B3_crd,
  double* __restrict__ B_vals,
  int C2_dimension,
  double* __restrict__ C_vals,
  uint64_t k
) {
  double tlA_val = 0.0;
  for (uint64_t lB = startIter; lB < maxIter; lB++) {
    uint64_t l = B3_crd[lB];
    uint64_t lC = k * C2_dimension + l;
    tlA_val += B_vals[lB] * C_vals[lC];
  }
  return tlA_val;
}

void HEARTBEAT_loop3(
  uint64_t maxIter,
  int A3_dimension,
  double* __restrict__ A_vals,
  int* __restrict__ B2_crd,
  int* __restrict__ B3_pos,
  int* __restrict__ B3_crd,
  double* __restrict__ B_vals,
  int C2_dimension,
  double* __restrict__ C_vals,
  uint64_t jB,
  uint64_t jA
) {
  for (uint64_t k = 0; k < maxIter; k++) {
    uint64_t kA = jA * A3_dimension + k;
    double tlA_val = 0.0;
    tlA_val += HEARTBEAT_loop4(B3_pos[jB], B3_pos[(jB + 1)], B3_crd, B_vals, C2_dimension, C_vals, k);
    A_vals[kA] = tlA_val;
  }
}

void HEARTBEAT_loop2(
  uint64_t startIter,
  uint64_t maxIter,
  int A2_dimension,
  int A3_dimension,
  double* __restrict__ A_vals,
  int* __restrict__ B2_crd,
  int* __restrict__ B3_pos,
  int* __restrict__ B3_crd,
  double* __restrict__ B_vals,
  int C1_dimension,
  int C2_dimension,
  double* __restrict__ C_vals,
  uint64_t i
) {
  for (uint64_t jB = startIter; jB < maxIter; jB++) {
    uint64_t j = B2_crd[jB];
    uint64_t jA = i * A2_dimension + j;
    HEARTBEAT_loop3(C1_dimension, A3_dimension, A_vals, B2_crd, B3_pos, B3_crd, B_vals, C2_dimension, C_vals, jB, jA);
  }
}

void HEARTBEAT_loop1(
  uint64_t maxIter,
  int A2_dimension,
  int A3_dimension,
  double* __restrict__ A_vals,
  int* __restrict__ B2_pos,
  int* __restrict__ B2_crd,
  int* __restrict__ B3_pos,
  int* __restrict__ B3_crd,
  double* __restrict__ B_vals,
  int C1_dimension,
  int C2_dimension,
  double* __restrict__ C_vals
) {
  for (uint64_t i = 0; i < maxIter; i++) {
    HEARTBEAT_loop2(B2_pos[i], B2_pos[(i + 1)], A2_dimension, A3_dimension, A_vals, B2_crd, B3_pos, B3_crd, B_vals, C1_dimension, C2_dimension, C_vals, i);
  }
}

void TTM_hbc(
  int A1_dimension,
  int A2_dimension,
  int A3_dimension,
  double* __restrict__ A_vals,
  int B1_dimension,
  int* __restrict__ B2_pos,
  int* __restrict__ B2_crd,
  int* __restrict__ B3_pos,
  int* __restrict__ B3_crd,
  double* __restrict__ B_vals,
  int C1_dimension,
  int C2_dimension,
  double* __restrict__ C_vals
) {
  HEARTBEAT_loop0(((A1_dimension * A2_dimension) * A3_dimension), A_vals);
  HEARTBEAT_loop1(B1_dimension, A2_dimension, A3_dimension, A_vals, B2_pos, B2_crd, B3_pos, B3_crd, B_vals, C1_dimension, C2_dimension, C_vals);
}

#endif

int compute(taco_tensor_t *A, taco_tensor_t *B, taco_tensor_t *C) {
  auto runs = 1;
  if (const auto env_p = std::getenv("RUNS")) {
    if (const auto env_p_p = std::atol(env_p)) {
      runs = env_p_p;
    }
  }
  int A1_dimension = (int)(A->dimensions[0]);
  int A2_dimension = (int)(A->dimensions[1]);
  int A3_dimension = (int)(A->dimensions[2]);
  double* __restrict__ A_vals = (double*)(A->vals);
  int B1_dimension = (int)(B->dimensions[0]);
  int* __restrict__ B2_pos = (int*)(B->indices[1][0]);
  int* __restrict__ B2_crd = (int*)(B->indices[1][1]);
  int* __restrict__ B3_pos = (int*)(B->indices[2][0]);
  int* __restrict__ B3_crd = (int*)(B->indices[2][1]);
  double* __restrict__ B_vals = (double*)(B->vals);
  int C1_dimension = (int)(C->dimensions[0]);
  int C2_dimension = (int)(C->dimensions[1]);
  double* __restrict__ C_vals = (double*)(C->vals);

#if defined(USE_BASELINE) || defined(USE_FORKJOIN)
  taskparts::benchmark_nativeforkjoin([&] (auto sched) {
#elif defined(USE_OPENMP)
  utility::run([&] {
#elif defined(USE_HB_COMPILER)
  run_bench([&] {
#endif

    for (int i = 0; i < runs; i++) {
#if defined(USE_BASELINE)
      TTM_baseline(A1_dimension, A2_dimension, A3_dimension, A_vals, B1_dimension, B2_pos, B2_crd, B3_pos, B3_crd, B_vals, C1_dimension, C2_dimension, C_vals);
#elif defined(USE_OPENMP)
      TTM_openmp(A1_dimension, A2_dimension, A3_dimension, A_vals, B1_dimension, B2_pos, B2_crd, B3_pos, B3_crd, B_vals, C1_dimension, C2_dimension, C_vals);
#elif defined(USE_HB_COMPILER)
      TTM_hbc(A1_dimension, A2_dimension, A3_dimension, A_vals, B1_dimension, B2_pos, B2_crd, B3_pos, B3_crd, B_vals, C1_dimension, C2_dimension, C_vals);
#endif
    }

#if defined(TEST_CORRECTNESS)
    test_correctness(A1_dimension, A2_dimension, A3_dimension, A_vals, B1_dimension, B2_pos, B2_crd, B3_pos, B3_crd, B_vals, C1_dimension, C2_dimension, C_vals);
#endif

#if defined(USE_BASELINE) || defined(USE_FORKJOIN)
  }, [&] (auto sched) {
  }, [&] (auto sched) {
  });
#else
  }, [&] {}, [&] {});
#endif

  return 0;
}
