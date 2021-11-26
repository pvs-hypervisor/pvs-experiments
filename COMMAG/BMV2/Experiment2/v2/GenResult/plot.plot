# Use gnuplot5 to run this script
#

LOOP = "#4671d5"; PVS = "#cc1414"; P4 = "#2eb519"
FONTL = ",22"
NFONT = "Helvetica, 16"
set auto x
set grid xtics ytics lw 1.6
set xtics 4
set ytics 25

set yrange[0:60]

set xlabel '{/Helvetica:Bold Time (s)}'
set ylabel '{/Helvetica:Bold Throughput (Mbps)}'
set key outside horizontal center top width -1.2
set terminal postscript eps enhanced color font 'Helvetica,20'
set size 1.0, 0.5
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

unset xlabel
set xlabel '{/Helvetica:Bold Time (s)}'
set ylabel "{/Helvetica:Bold Throughput (Mbps)}"

set label 4 "Legitimate" at 1.2,40
set label 5 "Malicious" at 12.8,40

set arrow 6 from 5.9,38 to 11.6,20 lw 4 lc rgb 'black'
set arrow 7 from 5.1,32 to 9.6,16 lw 4 lc rgb 'black'
set arrow 8 from 17.1,36 to 22.1,20 lw 4 lc rgb 'black'
set arrow 9 from 16.5,32 to 18.2,14.6 lw 4 lc rgb 'black'

set arrow from 11.749,0 to 11.749,60 nohead dt 2 lw 4 lc rgb 'black'
set arrow from 11.753,0 to 11.753,60 nohead dt 3 lw 4 lc rgb 'gray'
set arrow from 22.301,0 to 22.301,60 nohead dt 2 lw 4 lc rgb 'black'

set label 10 "(1)" at 9.232,67.06 font NFONT
set label 11 "(3)" at 11.253,67.06 font NFONT
set label 12 "(2)" at 17.969,67.06 font NFONT
set label 13 "(4)" at 21.801,67.06 font NFONT

plot    'table.csv' using 1:2 with linespoints notitle "H1" lw 4 ps 0 lt rgb PVS, \
	'table.csv' using 1:3 with linespoints notitle "H2" lw 4 ps 0 lt rgb P4, \
        NaN with linespoints title "H1" lw 4 ps 0 lt rgb PVS, \
        NaN with linespoints title "H2" lw 4 ps 0 lt rgb P4, \
        'table.csv' using 1:4 with linespoints title "H3" dt 2 lw 4 ps 0 lt rgb LOOP, \
	'table.csv' using 1:5 with linespoints title "H4" dt 2 lw 4 ps 0 lt rgb 'green', \
	NaN with lines title "Request" dt 2 lw 4 lt rgb 'black', \
	NaN with lines title "Reconfig" dt 3 lw 4 lt rgb 'gray'


