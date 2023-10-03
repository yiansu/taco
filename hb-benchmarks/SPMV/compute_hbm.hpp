#include <cstdint>

#define unlikely(expr) __builtin_expect(!!(expr), 0)
#define NUM_LEVELS  2
#define LEVEL_ZERO  0
#define LEVEL_ONE   1
#define CACHELINE   8
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

void HEARTBEAT_nest0_loop0(int A1_dimension, int *A2_pos, int *A2_crd, double *A_vals, double *x_vals, double *y_vals);
double HEARTBEAT_nest0_loop1(int *A2_pos, int *A2_crd, double *A_vals, double *x_vals, int32_t i);

int64_t HEARTBEAT_nest0_loop0_slice(uint64_t *cxts, uint64_t *constLiveIns, uint64_t myIndex, task_memory_t *tmem);
int64_t HEARTBEAT_nest0_loop1_slice(uint64_t *cxts, uint64_t *constLiveIns, uint64_t myIndex, task_memory_t *tmem);
typedef int64_t (*sliceTasksPointer)(uint64_t *, uint64_t *, uint64_t, task_memory_t *);
sliceTasksPointer slice_tasks_nest0[2] = {
  &HEARTBEAT_nest0_loop0_slice,
  &HEARTBEAT_nest0_loop1_slice
};

void HEARTBEAT_nest0_loop_1_0_leftover(uint64_t *cxts, uint64_t *constLiveIns, uint64_t myIndex, task_memory_t *tmem);
typedef void (*leftoverTasksPointer)(uint64_t *, uint64_t *, uint64_t, task_memory_t *);
leftoverTasksPointer leftover_tasks_nest0[1] = {
  &HEARTBEAT_nest0_loop_1_0_leftover
};

uint64_t leftover_selector_nest0(uint64_t receivingLevel, uint64_t splittingLevel) {
  return 0;
}

#if defined(RUN_HEARTBEAT)
  bool run_heartbeat = true;
#else
  bool run_heartbeat = false;
#endif

// Entry function
void SPMV_hbm(int A1_dimension, int *A2_pos, int *A2_crd, double *A_vals, double *x_vals, double *y_vals) {
  HEARTBEAT_nest0_loop0(A1_dimension, A2_pos, A2_crd, A_vals, x_vals, y_vals);
}

// Outlined loops
void HEARTBEAT_nest0_loop0(int A1_dimension, int *A2_pos, int *A2_crd, double *A_vals, double *x_vals, double *y_vals) {
  if (run_heartbeat) {
    run_heartbeat = false;

    // allocate const live-ins
    uint64_t constLiveIns[5];
    constLiveIns[0] = (uint64_t)A2_pos;
    constLiveIns[1] = (uint64_t)A2_crd;
    constLiveIns[2] = (uint64_t)A_vals;
    constLiveIns[3] = (uint64_t)x_vals;
    constLiveIns[4] = (uint64_t)y_vals;

    // allocate cxts
    uint64_t cxts[NUM_LEVELS * CACHELINE];

    // set start/max iterations for loop0
    cxts[LEVEL_ZERO * CACHELINE + START_ITER] = (uint64_t)0;
    cxts[LEVEL_ZERO * CACHELINE + MAX_ITER] = (uint64_t)A1_dimension;

    // allocate the task memory struct and initialize
    task_memory_t tmem;
    heartbeat_start(&tmem);

    // invoke loop0 in heartbeat form
    HEARTBEAT_nest0_loop0_slice(cxts, constLiveIns, 0, &tmem);

    run_heartbeat = true;
  } else {
    for (int32_t i = 0; i < A1_dimension; i++) { // row loop
      double tjy_val = 0.0;
      tjy_val += HEARTBEAT_nest0_loop1(A2_pos, A2_crd, A_vals, x_vals, i);
      y_vals[i] = tjy_val;
    }
  }
}

double HEARTBEAT_nest0_loop1(int *A2_pos, int *A2_crd, double *A_vals, double *x_vals, int32_t i) {
  double tjy_val = 0.0;
  for (int32_t jA = A2_pos[i]; jA < A2_pos[(i + 1)]; jA++) {
    int32_t j = A2_crd[jA];
    tjy_val += A_vals[jA] * x_vals[j];
  }
  return tjy_val;
}

// Transformed loops
int64_t HEARTBEAT_nest0_loop0_slice(uint64_t *cxts, uint64_t *constLiveIns, uint64_t myIndex, task_memory_t *tmem) {
  // load start/max iterations
  uint64_t startIter = cxts[LEVEL_ZERO * CACHELINE + START_ITER];
  uint64_t maxIter = cxts[LEVEL_ZERO * CACHELINE + MAX_ITER];

  // load const live-ins
  int *A2_pos = (int *)constLiveIns[0];
  double *y_vals = (double *)constLiveIns[4];

  // allocate reduction array (as live-out environment) for loop1
  double redArrLoop1LiveOut0[1 * CACHELINE];
  cxts[LEVEL_ONE * CACHELINE + LIVE_OUT] = (uint64_t)redArrLoop1LiveOut0;

  int64_t rc = 0;
  for (; startIter < maxIter; startIter++) {
    double tjy_val = 0.0;
    // store current iteration for loop0
    cxts[LEVEL_ZERO * CACHELINE + START_ITER] = startIter;
    // set start/max iterations for loop1
    cxts[LEVEL_ONE * CACHELINE + START_ITER] = (uint64_t)A2_pos[startIter];
    cxts[LEVEL_ONE * CACHELINE + MAX_ITER] = (uint64_t)A2_pos[startIter + 1];
    rc = HEARTBEAT_nest0_loop1_slice(cxts, constLiveIns, 0, tmem);
    tjy_val += redArrLoop1LiveOut0[0 * CACHELINE];
    y_vals[startIter] = tjy_val;

    // check the status of rc because, might not need
    // to call the loop_handler in the process of returnning up
    if (rc > 0) {
      break;
    }
  }

  return rc - 1;
}

int64_t HEARTBEAT_nest0_loop1_slice(uint64_t *cxts, uint64_t *constLiveIns, uint64_t myIndex, task_memory_t *tmem) {
  // load start/max iterations
  uint64_t startIter = cxts[LEVEL_ONE * CACHELINE + START_ITER];
  uint64_t maxIter = cxts[LEVEL_ONE * CACHELINE + MAX_ITER];

  // load const live-ins
  int *A2_crd = (int *)constLiveIns[1];
  double *A_vals = (double *)constLiveIns[2];
  double *x_vals = (double *)constLiveIns[3];

  // load reduction array for live-outs and create a private copy variable
  double *redArrLoop1LiveOut0 = (double *)cxts[LEVEL_ONE * CACHELINE + LIVE_OUT];
  double live_out_0 = 0.0;

  // allocate reduction array (as live-out environment) for kids of loop1
  double redArrLoop1LiveOut0Kids[2 * CACHELINE];
  cxts[LEVEL_ONE * CACHELINE + LIVE_OUT] = (uint64_t)redArrLoop1LiveOut0Kids;

  int64_t rc = 0;
#if defined(CHUNK_LOOP_ITERATIONS)
  uint64_t chunksize;
  for (; startIter < maxIter; startIter += chunksize) {
    chunksize = get_chunksize(tmem);
    uint64_t low = startIter;
    uint64_t high = maxIter < startIter + chunksize ? maxIter : startIter + chunksize;
    for (; low < high; low++) {
      int32_t j = A2_crd[low];
      live_out_0 += A_vals[low] * x_vals[j];
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
      cxts[LEVEL_ONE * CACHELINE + START_ITER] = low - 1;
      rc = loop_handler(
        cxts, constLiveIns, LEVEL_ONE, NUM_LEVELS, tmem,
        slice_tasks_nest0, leftover_tasks_nest0, &leftover_selector_nest0
      );
      if (rc > 0) {
        break;
      }
#if !defined(PROMOTION_INSERTION_OVERHEAD_ANALYSIS)
    }
#endif
#else
    if(unlikely(__rf_test())) {
      cxts[LEVEL_ONE * CACHELINE + START_ITER] = low - 1;
      __rf_handle_wrapper(
        rc, cxts, constLiveIns, LEVEL_ONE, NUM_LEVELS, tmem,
        slice_tasks_nest0, leftover_tasks_nest0, &leftover_selector_nest0
      );
      if (rc > 0) {
        break;
      }
    }
#endif
#endif
  }
#else
  for(; startIter < maxIter; startIter++) {
    int32_t j = A2_crd[startIter];
    live_out_0 += A_vals[startIter] * x_vals[j];

#if defined(ENABLE_HEARTBEAT)
#if defined(ENABLE_SOFTWARE_POLLING)
    if (unlikely(heartbeat_polling(tmem))) {
      cxts[LEVEL_ONE * CACHELINE + START_ITER] = startIter;
      rc = loop_handler(
        cxts, constLiveIns, LEVEL_ONE, NUM_LEVELS, tmem,
        slice_tasks_nest0, leftover_tasks_nest0, &leftover_selector_nest0
      );
      if (rc > 0) {
        break;
      }
    }
#else
    if(unlikely(__rf_test())) {
      cxts[LEVEL_ONE * CACHELINE + START_ITER] = startIter;
      __rf_handle_wrapper(
        rc, cxts, constLiveIns, LEVEL_ONE, NUM_LEVELS, tmem,
        slice_tasks_nest0, leftover_tasks_nest0, &leftover_selector_nest0
      );
      if (rc > 0) {
        break;
      }
    }
#endif
#endif
  }
#endif

  // rc from a leaf loop slice can only be 0 or 1,
  // however, when considering a non-leaf loop need to
  // consider the scenario of returning up
  // reduction
  if (rc == 0) {  // no heartbeat promotion happens
    redArrLoop1LiveOut0[myIndex * CACHELINE] = live_out_0;
  } else if (rc > 1) {  // splittingLevel < myLevel
    redArrLoop1LiveOut0[myIndex * CACHELINE] = live_out_0 + redArrLoop1LiveOut0Kids[0 * CACHELINE];
  } else {        // splittingLevel == myLevel
    redArrLoop1LiveOut0[myIndex * CACHELINE] = live_out_0 + redArrLoop1LiveOut0Kids[0 * CACHELINE] + redArrLoop1LiveOut0Kids[1 * CACHELINE];
  }

  // reset live-out environment
  cxts[LEVEL_ONE * CACHELINE + LIVE_OUT] = (uint64_t)redArrLoop1LiveOut0;

  return rc - 1;
}

// Leftover tasks
void HEARTBEAT_nest0_loop_1_0_leftover(uint64_t *cxts, uint64_t *constLiveIns, uint64_t myIndex, task_memory_t *tmem) {
  int64_t rc = 0;
  rc = HEARTBEAT_nest0_loop1_slice(cxts, constLiveIns, myIndex, tmem);
  if (rc > 0) {
    return;
  }

  cxts[LEVEL_ZERO * CACHELINE + START_ITER]++;
  rc = HEARTBEAT_nest0_loop0_slice(cxts, constLiveIns, myIndex, tmem);
  if (rc > 0) {
    return;
  }

  return;
}
