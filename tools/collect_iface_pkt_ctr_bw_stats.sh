if [ $# -ne 2 ]; then
        echo
        echo "usage: $0 [network-interface] [interval(seconds)]"
        echo
        echo e.g. $0 eth0 0.1
        echo
        echo shows packets-per-second
        exit
fi

IF=$1
INTERVAL=$2

echo "Experiment began `date`";
while true
do
        # fetch data
	DATA=`cat /sys/class/net/$IF/statistics/rx_packets /sys/class/net/$IF/statistics/tx_packets /sys/class/net/$IF/statistics/rx_bytes /sys/class/net/$IF/statistics/tx_bytes`;
        set -- $DATA;
        R1=$1
        T1=$2
        R1bytes=$3
	T1bytes=$4
        sleep $INTERVAL
	
	#fetch delta...
	DATA=`cat /sys/class/net/$IF/statistics/rx_packets /sys/class/net/$IF/statistics/tx_packets /sys/class/net/$IF/statistics/rx_bytes /sys/class/net/$IF/statistics/tx_bytes`;
	set -- $DATA;
        R2=$1
        T2=$2
        R2bytes=$3
        T2bytes=$4

	#compute the diff
	TXPPS=$(($T2 - $T1))
        RXPPS=$(($R2 - $R1))
        TXBPS=$(($T2bytes - $T1bytes))
        RXBPS=$(($R2bytes - $R1bytes))

	#output to stdout
        echo "`date +%s` TX $IF: $TXPPS pkts/s $TXBPS bps RX $IF: $RXPPS pkts/s $RXBPS bps"
 done
