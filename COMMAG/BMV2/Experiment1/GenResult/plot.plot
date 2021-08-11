# Use gnuplot5 to run this script
#

LOOP = "#4671d5"; PVS = "#cc1414"; P4 = "#2eb519"
FONTL = ",22"
set auto x
set grid xtics ytics lw 1.6
set xtics 1
#set ytics 20

#set yrange[0:100]
#set xrange[12:24]
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
set output 'plot.eps'

set datafile separator ';'

set style rectangle fs solid border lw 4
set arrow from 0.968,0 to 0.968,120 nohead dt 3 lw 3 lc rgb 'orange'
set arrow from 1.012,0 to 1.012,120 nohead dt 3 lw 3 lc rgb 'gray'
set arrow from 1.016,0 to 1.016,120 nohead dt 3 lw 3 lc rgb 'orange'
set arrow from 1.273,0 to 1.273,120 nohead dt 3 lw 3 lc rgb 'gray'

set arrow from 5.909,0 to 5.909,120 nohead dt 3 lw 3 lc rgb 'gray'
set arrow from 5.910,0 to 5.910,120 nohead dt 3 lw 3 lc rgb 'orange'
set arrow from 5.931,0 to 5.931,120 nohead dt 3 lw 3 lc rgb 'gray'

plot    'table.csv' using 1:2 with linespoints title "H1" lw 4 ps 0 lt rgb PVS, \
	'table.csv' using 1:3 with linespoints title "H2" lw 4 ps 0 lt rgb P4, \
	'table.csv' using 1:4 with linespoints title "H3" lw 4 ps 0 lt rgb LOOP, \
        NaN with lines title "Miss" dt 3 lw 4 lt rgb 'orange', \
        NaN with lines title "Reconfig" dt 3 lw 4 lt rgb 'gray'
