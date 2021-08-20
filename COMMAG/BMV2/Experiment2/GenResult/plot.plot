# Use gnuplot5 to run this script
#

LOOP = "#4671d5"; PVS = "#cc1414"; P4 = "#2eb519"
FONTL = ",22"
set auto x
set grid xtics ytics lw 1.6
set xtics 4
set ytics 25

set yrange[0:60]

set xlabel '{/Helvetica:Bold Time (s)}'
set ylabel '{/Helvetica:Bold Throughput (Mbps)}'
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
set arrow 1 from 9.732,0 to 9.732,60 nohead dt 2 lw 4 lc rgb 'black'
set arrow 2 from 9.736 to 9.736,60 nohead dt 3 lw 4 lc rgb 'gray'
set arrow 3 from 18.469,0 to 18.469,60 nohead dt 2 lw 4 lc rgb 'black'

set multiplot layout 2,1 \
              margins 0.1,0.96,0.14,0.7 \
              spacing 0.08,0.08

unset xlabel
set ylabel " "

set label 4 "Legitimate" at 4,40
set label 5 "Malicious" at 12,40

set arrow 6 from 7.5,32 to 9.6,20 lw 4 lc rgb 'black'
set arrow 7 from 15,32 to 18.2,20 lw 4 lc rgb 'black'

plot    'table.csv' using 1:2 with linespoints notitle "H1" lw 4 ps 0 lt rgb PVS, \
	'table.csv' using 1:3 with linespoints notitle "H2" lw 4 ps 0 lt rgb P4

unset arrow 1
unset arrow 2
unset arrow 3
unset label 4
unset label 5
unset arrow 6
unset arrow 7

set label 4 "Legitimate" at 6,40
set label 5 "Malicious" at 16,40

set arrow 6 from 9.5,32 to 11.6,20 lw 4 lc rgb 'black'
set arrow 7 from 19,32 to 22.2,20 lw 4 lc rgb 'black'

set arrow from 11.749,0 to 11.749,60 nohead dt 2 lw 4 lc rgb 'black'
set arrow from 11.753,0 to 11.753,60 nohead dt 3 lw 4 lc rgb 'gray'
set arrow from 22.301,0 to 22.301,60 nohead dt 2 lw 4 lc rgb 'black'

set xlabel '{/Helvetica:Bold Time (s)}'
set label '{/Helvetica:Bold Throughput (Mbps)}' at -2.5,30 rotate by 90
plot    NaN with linespoints title "H1" lw 4 ps 0 lt rgb PVS, \
	NaN with linespoints title "H2" lw 4 ps 0 lt rgb P4, \
        'table.csv' using 1:4 with linespoints title "H3" dt 2 lw 4 ps 0 lt rgb LOOP, \
	'table.csv' using 1:5 with linespoints title "H4" dt 2 lw 4 ps 0 lt rgb 'green', \
	NaN with lines title "Request" dt 2 lw 4 lt rgb 'black', \
	NaN with lines title "Reconfig" dt 3 lw 4 lt rgb 'gray'
