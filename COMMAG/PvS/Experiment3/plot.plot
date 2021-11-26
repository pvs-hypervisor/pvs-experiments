# Use gnuplot5 to run this script
#

LOOP = "#4671d5"; PVS = "#cc1414"; P4 = "#2eb519"
FONTL = ",22"
set auto x
set grid xtics ytics lw 1.6
set xtics 2
set ytics 5

set yrange[0:12]

set xlabel '{/Helvetica:Bold Time (s)}'
set ylabel '{/Helvetica:Bold Throughput (Gbps)}'
set key outside horizontal center top width -1.2 height 8
set terminal postscript eps enhanced color font 'Helvetica,20'
set size ratio 0.18
set title font FONTL
set xtics font FONTL
set ytics font FONTL
set xlabel font FONTL
set ylabel font FONTL
set output 'plot.eps'

set datafile separator ';'

set style rectangle fs solid border lw 4
set arrow 2 from 3.267,0 to 3.267,12 nohead dt 3 lw 4 lc rgb 'gray'

set multiplot layout 2,1 \
              margins 0.1,0.96,0.14,0.7 \
              spacing 0.08,0.08

unset xlabel
set ylabel " "

plot    'table.csv' using 1:2 with linespoints notitle "H1" lw 4 ps 0 lt rgb PVS, \
	'table.csv' using 1:4 with linespoints notitle "H2" lw 4 ps 0 lt rgb P4, \
	

set xlabel '{/Helvetica:Bold Time (s)}'
set label '{/Helvetica:Bold Throughput (Gbps)}' at -1.8,6 rotate by 90

unset arrow 2
set arrow 3 from 8.183,0 to 8.183,12 nohead dt 3 lw 4 lc rgb 'gray'
set arrow 5 from 14.357,0 to 14.357,12 nohead dt 3 lw 4 lc rgb 'gray'

plot    NaN with linespoints title "H1" lw 4 ps 0 lt rgb PVS, \
	NaN with linespoints title "H2" lw 4 ps 0 lt rgb P4, \
        'table.csv' using 1:3 with linespoints title "H3" dt 2 lw 4 ps 0 lt rgb LOOP, \
        'table.csv' using 1:5 with linespoints title "H4" dt 2 lw 4 ps 0 lt rgb 'green', \
	NaN with lines title "Reconfig" dt 3 lw 4 lt rgb 'gray'
