#!/bin/bash

DATASET=$1
START=$2
END=$3
RATE=$4

curr_timestamp=0;
curr_tx_pps=0;
curr_tx_bps=0;
curr_rx_pps=0;
curr_rx_bps=0;
ctr=0;

while read line; do 
  tokens=( $line );
  if [ ${tokens[0]} -gt $END ]; then
    break;

  elif [ ${tokens[0]} -lt $START ]; then
    continue;

  elif [ ${tokens[0]} -ne $curr_timestamp ]; then
    # dump line
    if [ ${curr_timestamp} -ne 0 ]; then
      # compute avg. to compensate for missing samples
      curr_tx_pps=`echo $curr_tx_pps*$RATE/$ctr | bc -l | cut -f1 -d.`;
      curr_tx_bps=`echo $curr_tx_bps*$RATE/$ctr | bc -l | cut -f1 -d.`;
      curr_rx_pps=`echo $curr_rx_pps*$RATE/$ctr | bc -l | cut -f1 -d.`;
      curr_rx_bps=`echo $curr_rx_bps*$RATE/$ctr | bc -l | cut -f1 -d.`;
      echo ${curr_timestamp} ${curr_tx_pps} ${curr_tx_bps} ${curr_rx_pps} ${curr_rx_bps};
    fi;
    curr_timestamp=${tokens[0]};
    curr_tx_pps=${tokens[3]};
    curr_tx_bps=${tokens[5]};
    curr_rx_pps=${tokens[9]};
    curr_rx_bps=${tokens[11]};
    ctr=1;

  else
    let curr_tx_pps+=tokens[3];
    let curr_tx_bps+=tokens[5];
    let curr_rx_pps+=tokens[9];
    let curr_rx_bps+=tokens[11];
    let ctr++;
  fi;
done < <(grep TX ${DATASET});
