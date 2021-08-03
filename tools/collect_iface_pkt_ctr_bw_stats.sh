if [ $# -ne 2 ]; then
        echo
        echo "usage: $0 [network-interface] [interval(miliseconds)]"
        echo
        echo e.g. $0 eth0 100
        echo
        echo shows packets-per-second. interval MUST BE multiple of 100
        exit
fi

IF=$1
INTERVAL=$2
SLEEP_INT=`echo $2/1000 | bc -l`

echo "Experiment began `date` (interval $INTERVAL sleep_int $SLEEP_INT)";

# fetch first data
DATA=`cat /sys/class/net/$IF/statistics/rx_packets \
	/sys/class/net/$IF/statistics/tx_packets \
	/sys/class/net/$IF/statistics/rx_bytes \
	/sys/class/net/$IF/statistics/tx_bytes`;
set -- $DATA;
Rcurr=$1
Tcurr=$2
Rcurrbytes=$3
Tcurrbytes=$4

while true
do
	sleep $SLEEP_INT

	#save previous data
        Rprev=$Rcurr
	Tprev=$Tcurr
	Rprevbytes=$Rcurrbytes
	Tprevbytes=$Tcurrbytes

        # fetch next data
	DATA=`cat /sys/class/net/$IF/statistics/rx_packets \
		/sys/class/net/$IF/statistics/tx_packets \
		/sys/class/net/$IF/statistics/rx_bytes \
		/sys/class/net/$IF/statistics/tx_bytes`;
        set -- $DATA;
	Rcurr=$1
	Tcurr=$2
	Rcurrbytes=$3
	Tcurrbytes=$4

	#compute the diff
	TXPPS=$((($Tcurr - $Tprev)*1000/$INTERVAL))
	RXPPS=$((($Rcurr - $Rprev)*1000/$INTERVAL))
	TXBPS=$((($Tcurrbytes - $Tprevbytes)*1000/$INTERVAL))
	RXBPS=$((($Rcurrbytes - $Rprevbytes)*1000/$INTERVAL))

	#output to stdout
        echo "`date +%s` TX $IF: $TXPPS pkts/s $TXBPS bps RX $IF: $RXPPS pkts/s $RXBPS bps"
 done
