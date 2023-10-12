#include <cstdint>

#define unlikely(expr) __builtin_expect(!!(expr), 0)
#define NUM_LEVELS  1
#define LEVEL_ZERO  0
#define CACHELINE   4
#define START_ITER  0
#define MAX_ITER    1
#define LIVE_IN     2
#define LIVE_OUT    3

#if defined(ENABLE_ROLLFORWARD)
extern volatile int __rf_signal;

extern "C" {

__attribute__((used))
__attribute__((always_inline))
static bool __rf_test (void) {
  int yes;
  asm volatile ("movl $__rf_signal, %0" : "=r" (yes) : : );
  return yes == 1;
}

}
#endif

void HEARTBEAT_loop0(int *B2_pos, int *B2_crd, int *B3_pos, int *B3_crd, int D2_dimension, int A2_dimension, int C2_dimension, double *A_vals, double *B_vals, double *C_vals, double *D_vals, uint64_t i);

int64_t HEARTBEAT_loop0_slice(uint64_t startIter, uint64_t maxIter, uint64_t *cxts, uint64_t *constLiveIns, uint64_t myIndex, task_memory_t *tmem);
typedef int64_t (*sliceTasksPointer)(uint64_t, uint64_t, uint64_t *, uint64_t *, uint64_t, task_memory_t *);
sliceTasksPointer slice_tasks[1] = {
  &HEARTBEAT_loop0_slice
};

#if defined(RUN_HEARTBEAT)
  bool run_heartbeat = true;
#else
  bool run_heartbeat = false;
#endif

// Entry function
void MTTKRP_hbm(int B1_dimension, int *B2_pos, int *B2_crd, int *B3_pos, int *B3_crd, int D2_dimension, int A2_dimension, int C2_dimension, double *A_vals, double *B_vals, double *C_vals, double *D_vals) {
  for (uint64_t i = 0; i < B1_dimension; i++) {
    HEARTBEAT_loop0(B2_pos, B2_crd, B3_pos, B3_crd, D2_dimension, A2_dimension, C2_dimension, A_vals, B_vals, C_vals, D_vals, i);
  }
}

// Outlined loops
void HEARTBEAT_loop0(int *B2_pos, int *B2_crd, int *B3_pos, int *B3_crd, int D2_dimension, int A2_dimension, int C2_dimension, double *A_vals, double *B_vals, double *C_vals, double *D_vals, uint64_t i) {
  if (run_heartbeat) {
    run_heartbeat = false;

    // allocate const live-ins
    uint64_t constLiveIns[11];
    constLiveIns[0] = (uint64_t)B2_crd;
    constLiveIns[1] = (uint64_t)B3_pos;
    constLiveIns[2] = (uint64_t)B3_crd;
    constLiveIns[3] = (uint64_t)D2_dimension;
    constLiveIns[4] = (uint64_t)A2_dimension;
    constLiveIns[5] = (uint64_t)C2_dimension;
    constLiveIns[6] = (uint64_t)A_vals;
    constLiveIns[7] = (uint64_t)B_vals;
    constLiveIns[8] = (uint64_t)C_vals;
    constLiveIns[9] = (uint64_t)D_vals;
    constLiveIns[10] = (uint64_t)i;

    // allocate cxts
    uint64_t cxts[NUM_LEVELS * CACHELINE];

    // allocate the task memory struct and initialize
    task_memory_t tmem;
    heartbeat_start(&tmem);

    // invoke loop0 in heartbeat form
    HEARTBEAT_loop0_slice((uint64_t)B2_pos[i], (uint64_t)B2_pos[(i + 1)], cxts, constLiveIns, 0, &tmem);

    run_heartbeat = true;
  } else {
    for (uint64_t kB = B2_pos[i]; kB < B2_pos[(i + 1)]; kB++) {
      uint64_t k = B2_crd[kB];
      for (uint64_t lB = B3_pos[kB]; lB < B3_pos[(kB + 1)]; lB++) {
        uint64_t l = B3_crd[lB];
        for (uint64_t j = 0; j < D2_dimension; j++) {
          uint64_t jA = i * A2_dimension + j;
          uint64_t jC = k * C2_dimension + j;
          uint64_t jD = l * D2_dimension + j;
          A_vals[jA] = A_vals[jA] + (B_vals[lB] * C_vals[jC]) * D_vals[jD];
        }
      }
    }
  }
}

// Transformed loops
int64_t HEARTBEAT_loop0_slice(uint64_t startIter, uint64_t maxIter, uint64_t *cxts, uint64_t *constLiveIns, uint64_t myIndex, task_memory_t *tmem) {
  // get cxts offset
  uint64_t offset = tmem->startingLevel;

  // store max iteration
  cxts[(LEVEL_ZERO-offset) * CACHELINE + MAX_ITER] = maxIter;

  // load const live-ins
  int *B2_crd = (int *)constLiveIns[0];
  int *B3_pos = (int *)constLiveIns[1];
  int *B3_crd = (int *)constLiveIns[2];
  int D2_dimension = (int)constLiveIns[3];
  int A2_dimension = (int)constLiveIns[4];
  int C2_dimension = (int)constLiveIns[5];
  double *A_vals = (double *)constLiveIns[6];
  double *B_vals = (double *)constLiveIns[7];
  double *C_vals = (double *)constLiveIns[8];
  double *D_vals = (double *)constLiveIns[9];
  uint64_t i = (uint64_t)constLiveIns[10];

  int64_t rc = 0;
#if defined(CHUNK_LOOP_ITERATIONS)
  uint64_t chunksize;
  for (; startIter < maxIter; startIter += chunksize) {
    chunksize = get_chunksize(tmem);
    uint64_t low = startIter;
    uint64_t high = maxIter < startIter + chunksize ? maxIter : startIter + chunksize;
    for (; low < high; low++) {
      uint64_t k = B2_crd[low];
      for (uint64_t lB = B3_pos[low]; lB < B3_pos[(low + 1)]; lB++) {
        uint64_t l = B3_crd[lB];
        for (uint64_t j = 0; j < D2_dimension; j++) {
          uint64_t jA = i * A2_dimension + j;
          uint64_t jC = k * C2_dimension + j;
          uint64_t jD = l * D2_dimension + j;
          A_vals[jA] = A_vals[jA] + (B_vals[lB] * C_vals[jC]) * D_vals[jD];
        }
      }
    }

#if defined(ENABLE_HEARTBEAT)
#if !defined(PROMOTION_INSERTION_OVERHEAD_ANALYSIS)
    if (update_and_has_remaining_chunksize(tmem, high - startIter, chunksize)) {
      break;
    }
#endif

#if defined(ENABLE_SOFTWARE_POLLING)
#if !defined(PROMOTION_INSERTION_OVERHEAD_ANALYSIS)
    if (unlikely(heartbeat_polling(tmem))) {
#endif
      cxts[(LEVEL_ZERO-offset) * CACHELINE + START_ITER] = low - 1;
      rc = loop_handler(
        cxts, constLiveIns, LEVEL_ZERO, NUM_LEVELS, tmem,
        slice_tasks, nullptr, nullptr
      );
      if (rc > 0) {
        break;
      }
#if !defined(PROMOTION_INSERTION_OVERHEAD_ANALYSIS)
    }
#endif
#else
    if(unlikely(__rf_test())) {
      cxts[(LEVEL_ZERO-offset) * CACHELINE + START_ITER] = low - 1;
      __rf_handle_wrapper(
        rc, cxts, constLiveIns, LEVEL_ZERO, NUM_LEVELS, tmem,
        slice_tasks, nullptr, nullptr
      );
      if (rc > 0) {
        break;
      }
    }
#endif
#endif
  }
#else

  for (; startIter < maxIter; startIter++) {
    uint64_t k = B2_crd[startIter];
    for (uint64_t lB = B3_pos[startIter]; lB < B3_pos[(startIter + 1)]; lB++) {
      uint64_t l = B3_crd[lB];
      for (uint64_t j = 0; j < D2_dimension; j++) {
        uint64_t jA = i * A2_dimension + j;
        uint64_t jC = k * C2_dimension + j;
        uint64_t jD = l * D2_dimension + j;
        A_vals[jA] = A_vals[jA] + (B_vals[lB] * C_vals[jC]) * D_vals[jD];
      }
    }

#if defined(ENABLE_HEARTBEAT)
#if defined(ENABLE_SOFTWARE_POLLING)
    if (unlikely(heartbeat_polling(tmem))) {
      cxts[(LEVEL_ZERO-offset) * CACHELINE + START_ITER] = startIter;
      rc = loop_handler(
        cxts, constLiveIns, LEVEL_ZERO, NUM_LEVELS, tmem,
        slice_tasks, nullptr, nullptr
      );
      if (rc > 0) {
        break;
      }
    }
#else
    if(unlikely(__rf_test())) {
      cxts[(LEVEL_ZERO-offset) * CACHELINE + START_ITER] = startIter;
      __rf_handle_wrapper(
        rc, cxts, constLiveIns, LEVEL_ZERO, NUM_LEVELS, tmem,
        slice_tasks, nullptr, nullptr
      );
      if (rc > 0) {
        break;
      }
    }
#endif
#endif
  }
#endif

  return rc - 1;
}
