# Use gnuplot5 to run this script
#

LOOP = "#4671d5"; PVS = "#cc1414"; P4 = "#2eb519"
FONTL = ",22"
set auto x
set grid xtics ytics lw 1.6
#set xtics 60
#set ytics 20

#set yrange[0:100]
set xrange[0:34]
set xlabel '{/Helvetica:Bold Time (s)}'
set ylabel '{/Helvetica:Bold Throughput (Mbps)}'
set key outside horizontal center top width -1.2
set terminal postscript eps enhanced color font 'Helvetica,20'
set size 1, 0.6
set title font FONTL
set xtics font FONTL
set ytics font FONTL
set xlabel font FONTL
set ylabel font FONTL
set output 'bandwidth_vm.eps'

set style rectangle fs solid border lw 4

plot '../dataset/bw_eth0.txt.parsed' using ($1-1581132882):($3/1024/1024*8) with linespoints title "VM 1" lw 4 pt 4 ps 1 lt rgb PVS, \
        '../dataset/bw_eth1.txt.parsed' using ($1-1581132882):($5/1024/1024*8) with linespoints title "VM 2" lw 4 pt 4 ps 1 lt rgb P4, \
        '../dataset/bw_eth2.txt.parsed' using ($1-1581132882):($5/1024/1024*8) with linespoints title "VM 3" lw 4 pt 4 ps 1 lt rgb LOOP

