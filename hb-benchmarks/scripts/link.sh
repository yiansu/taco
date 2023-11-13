#!/bin/bash

for bench in `ls` ; do
  if ! test -d $bench ; then
    continue ;
  fi

  if [ $bench == "scripts" ] ; then
    continue ;
  fi

  if [ $bench == "commons" ] ; then
    continue ;
  fi

  if [ $bench == "inputs" ] ; then
    continue ;
  fi

  cd $bench ;
  
  ln -s ../commons/* . ;
  ln -s ../inputs . ;

  cd ../ ;

done
