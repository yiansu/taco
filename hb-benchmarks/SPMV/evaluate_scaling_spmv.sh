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
mkdir -p ${ROOT_DIR}/SPMV/results/${experiment};

########################################################
# experiment sections
baseline=false
hbc_acc=false     # software polling + acc
hbc_static=false  # software polling + static chunksize
hbc_rf=false      # rollforward + interrupt ping thread
hbc_rf_kmod=false # rollforward + kernel module
openmp=false

# benchmark targetted
benchmarks=(SPMV)
########################################################

function collect_matrix {
  local results_path=${1}
  local input=${2}
  local matrix_name=${3}
  local output=${results_path}/${keyword}_${matrix_name}.txt

  cat ${input} | grep -w ${keyword} | awk '{ print $2 }' | tr -d '},' >> ${output} ;
}

function run_and_collect {
  local technique=${1}
  local results_path=${2}
  mkdir -p ${results_path} ;

  for matrix in `ls matrices` ; do
    local matrix_name=`echo "${matrix}" | cut -d. -f1`
    local output=${results_path}/output_${matrix_name}.txt

    if [ ${technique} == "baseline" ] ; then
      # burnP6
      killall burnP6 &> /dev/null ;

      for coreID in `seq -s' ' 4 2 10` ; do
        taskset -c ${coreID} burnP6 &
      done

      for i in `seq 1 3` ; do
        taskset -c 2 make run_baseline INPUT_MATRIX="matrices/${matrix}" >> ${output} ;
      done

      # kill burnP6
      killall burnP6 &> /dev/null ;

    elif [ ${technique} == "openmp" ] ; then
      for i in `seq 1 10` ; do
        WORKERS=56 \
        numactl --physcpubind=0-55 --interleave=all \
        make run_openmp INPUT_MATRIX="matrices/${matrix}" >> ${output} ;
      done

    else
      for i in `seq 1 10` ; do
        WORKERS=56 \
        CPU_FREQUENCY_KHZ=${cpu_frequency_khz} \
        KAPPA_USECS=${heartbeat_interval} \
        numactl --physcpubind=0-55 --interleave=all \
        make run_hbc INPUT_MATRIX="matrices/${matrix}" >> ${output} ;
      done
    
    fi

    collect_matrix ${results_path} ${output} ${matrix_name} ;

  done
}

# main script
pushd . ;

# preparation
cd ${ROOT_DIR} ;

# run experiment per benchmark
for benchmark in ${benchmarks[@]} ; do

  results=${ROOT_DIR}/SPMV/results/${experiment}
  mkdir -p ${results} ;

  cd ${benchmark} ;
  clean ;

  # baseline
  if [ ${baseline} = true ] ; then
    clean ; make baseline &> /dev/null ;
    run_and_collect baseline ${results}/baseline ;
  fi

  # hbc_acc
  if [ ${hbc_acc} = true ] ; then
    clean ; make hbc ACC=true CHUNKSIZE=1 &> /dev/null ;
    run_and_collect hbc_acc ${results}/hbc_acc ;
  fi

  # hbc_rf
  if [ ${hbc_rf} = true ] ; then
    clean ; make hbc ENABLE_ROLLFORWARD=true CHUNK_LOOP_ITERATIONS=false &> /dev/null ;
    run_and_collect hbc_rf ${results}/hbc_rf ;
  fi

  # openmp
  if [ ${openmp} = true ] ; then
    omp_schedules=(STATIC DYNAMIC GUIDED)
    for omp_schedule in ${omp_schedules[@]} ; do
      clean ; make openmp OMP_SCHEDULE=${omp_schedule} &> /dev/null ;
      run_and_collect openmp ${results}/openmp_`echo -e ${omp_schedule} | tr '[:upper:]' '[:lower:]'` ;
    done
  fi

  clean ;
  cd ../ ;

done

popd ;
