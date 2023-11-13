#!/bin/bash

# environment
ROOT_DIR=`git rev-parse --show-toplevel`/hb-benchmarks
source /nfs-scratch/yso0488/heartbeatcompiler0/enable ;
source /nfs-scratch/yso0488/taco/enable ;
source /project/extra/burnCPU/enable ;
source common.sh ;

# experiment
experiment=scaling
keyword=exectime
mkdir -p ${ROOT_DIR}/evaluation/results/${experiment};

########################################################
# experiment sections
baseline=false
hbc_acc=false     # software polling + acc
hbc_static=false  # software polling + static chunksize
hbc_rf=false      # rollforward + interrupt ping thread
hbc_rf_kmod=false # rollforward + kernel module
forkjoin=false
openmp=false

hbm_0_2=false     # this applies to MTTKRP only

# benchmark targetted
benchmarks=(TTM TTV)
########################################################

function run_and_collect {
  local technique=${1}
  local results_path=${2}
  mkdir -p ${results_path} ;
  local output=${results_path}/output.txt

  if [ ${technique} == "baseline" ] ; then
    # burnP6
    killall burnP6 &> /dev/null ;

    for coreID in `seq -s' ' 4 2 10` ; do
      taskset -c ${coreID} burnP6 &
    done

    for i in `seq 1 3` ; do
      taskset -c 2 make run_baseline >> ${output} ;
    done

    # kill burnP6
    killall burnP6 &> /dev/null ;

  elif [ ${technique} == "openmp" ] ; then
    for i in `seq 1 10` ; do
      WORKERS=56 \
      numactl --physcpubind=0-55 --interleave=all \
      make run_openmp >> ${output} ;
    done

  elif [ ${technique} == "forkjoin" ] ; then
    for i in `seq 1 10` ; do
      WORKERS=56 \
      timeout ${timeout} \
      numactl --physcpubind=0-55 --interleave=all \
      make run_forkjoin >> ${output} ;
    done

  elif [ ${technique} == "hbm" ] ; then
    for i in `seq 1 10` ; do
      WORKERS=56 \
      CPU_FREQUENCY_KHZ=${cpu_frequency_khz} \
      KAPPA_USECS=${heartbeat_interval} \
      numactl --physcpubind=0-55 --interleave=all \
      make run_hbm >> ${output} ;
    done

  else
    for i in `seq 1 10` ; do
      WORKERS=56 \
      CPU_FREQUENCY_KHZ=${cpu_frequency_khz} \
      KAPPA_USECS=${heartbeat_interval} \
      numactl --physcpubind=0-55 --interleave=all \
      make run_hbc >> ${output} ;
    done

  fi

  collect ${results_path} ${output} ;
}

# main script
pushd . ;

# preparation
cd ${ROOT_DIR} ;

# run experiment per benchmark
for benchmark in ${benchmarks[@]} ; do

  results=${ROOT_DIR}/evaluation/results/${experiment}/${benchmark}
  mkdir -p ${results} ;

  cd ${benchmark} ;
  clean ;

  # baseline
  if [ ${baseline} = true ] ; then
    clean ; make baseline INPUT_SIZE=${input_size} &> /dev/null ;
    run_and_collect baseline ${results}/baseline ;
  fi

  # hbc_acc
  if [ ${hbc_acc} = true ] ; then
    clean ; make hbc INPUT_SIZE=${input_size} ACC=true CHUNKSIZE=1 &> /dev/null ;
    run_and_collect hbc_acc ${results}/hbc_acc_${parallel_level} ;
  fi

  # hbc_rf
  if [ ${hbc_rf} = true ] ; then
    clean ; make hbc INPUT_SIZE=${input_size} ENABLE_ROLLFORWARD=true CHUNK_LOOP_ITERATIONS=false &> /dev/null ;
    run_and_collect hbc_rf ${results}/hbc_rf ;
  fi

  # forkjoin
  if [ ${forkjoin} = true ] ; then
    clean ; make forkjoin INPUT_SIZE=${input_size} &> /dev/null ;
    run_and_collect forkjoin ${results}/forkjoin ;
  fi

  # openmp
  if [ ${openmp} = true ] ; then
    omp_schedules=(STATIC DYNAMIC GUIDED)
    for omp_schedule in ${omp_schedules[@]} ; do
      clean ; make openmp INPUT_SIZE=${input_size} OMP_SCHEDULE=${omp_schedule} &> /dev/null ;
      run_and_collect openmp ${results}/openmp_`echo -e ${omp_schedule} | tr '[:upper:]' '[:lower:]'` ;
    done
  fi

  # hbm_0_2
  if [ ${hbm_0_2} = true ] ; then
    clean ; make hbm INPUT_SIZE=${input_size} ACC=true CHUNKSIZE=1 &> /dev/null ;
    run_and_collect hbm ${results}/hbm_0_2 ;
  fi

  clean ;
  cd ../ ;

done

popd ;
