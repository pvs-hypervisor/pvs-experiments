# Use gnuplot5 to run this script
#

L2 = "#4671d5"; PVS = "#cc1414"; ROUTER = "#2eb519"; FIREWALL = "#bd5e06";
FONTL = ",22"
set auto x
set grid xtics ytics lw 1.6
set xtics 1
set ytics 10

set offsets 0.1, 0.1, 0, 0

#set mytics 2

#set grid mytics  # draw lines for each ytics and mytics

set yrange[0:40]
set xrange[1:5]
set xlabel '{/Helvetica:Bold Injected Packets}'
set ylabel '{/Helvetica:Bold Throughput (Gbps)}'
set key outside horizontal center top width 0
set terminal postscript eps enhanced color font 'Helvetica,20'
set size 1, 0.6
set title font FONTL
set xtics font FONTL
set ytics font FONTL
set xlabel font FONTL
set ylabel font FONTL
set output 'bandwidth.eps'

set style rectangle fs solid border lw 4

plot 'l2_media.txt' using ($0+1):($1/1000000000) with lines title "L2" dt 1 lw 2 lt rgb L2, \
     'router_media.txt' using ($0+1):($1/1000000000) with lines title "Router" dt 1 lw 2 lt rgb ROUTER, \
     'firewall_media.txt' using ($0+1):($1/1000000000) with lines title "Firewall" dt 1 lw 2 lt rgb FIREWALL, \
     'pvs_total_media.txt' using ($0+1):($1/1000000000) with lines title "PvS - Total" dt 1 lw 2 lt rgb PVS
