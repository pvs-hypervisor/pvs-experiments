# Use gnuplot5 to run this script
#

LOOP = "#4671d5"; PVS = "#cc1414"; P4 = "#2eb519"
FONTL = ",22"
NFONT = "Helvetica, 16"
set auto x
set grid xtics ytics lw 1.6
set xtics 2
set ytics 2

set yrange[0:12]

set ylabel '{/Helvetica:Bold Throughput (Gbps)}'
set key outside horizontal center top width -1.2
set terminal postscript eps enhanced color font 'Helvetica,20'
set title font FONTL
set xtics font FONTL
set ytics font FONTL
set xlabel font FONTL
set ylabel font FONTL
set output 'plot.eps'
set size 1.0, 0.5
#set size ratio 0.25

set datafile separator ';'

set style rectangle fs solid border lw 4
set arrow 1 from 3.105,0 to 3.105,12 nohead dt 2 lw 4 lc rgb 'black'
set arrow 2 from 3.106,0 to 3.106,12 nohead dt 3 lw 4 lc rgb 'gray'
set arrow 3 from 12.986,0 to 12.986,12 nohead dt 2 lw 4 lc rgb 'black'

unset xlabel
set xlabel '{/Helvetica:Bold Time (s)}'
set ylabel "{/Helvetica:Bold Throughput (Gbps)}"

set arrow 8 from 4.6,5 to 3.2,4.2 lw 4 lc rgb 'black'
set arrow 9 from 11.2,5 to 12.8,2.8 lw 4 lc rgb 'black'
	
set arrow 4 from 8.502,0 to 8.502,12 nohead dt 2 lw 4 lc rgb 'black'
set arrow 5 from 8.502,0 to 8.502,12 nohead dt 3 lw 4 lc rgb 'gray'
set arrow 6 from 14.416,0 to 14.416,12 nohead dt 2 lw 4 lc rgb 'black'

set label 6 "Legitimate" at 4.8,6 font ",16"
set label 7 "Malicious" at 9.4,6 font ",16"
set arrow 10 from 7.0,5 to 8.4,4.2 lw 4 lc rgb 'black'
set arrow 11 from 11.5,5.4 to 14.1,4.2 lw 4 lc rgb 'black'

set label 10 "(3)" at 8.202,13.06 font NFONT
set label 11 "(1)" at 2.805,13.06 font NFONT
set label 12 "(4)" at 14.116,13.06 font NFONT
set label 13 "(2)" at 12.686,13.06 font NFONT

plot    'table.csv' using 1:2 with linespoints notitle "H1" lw 4 ps 0 lt rgb PVS, \
	'table.csv' using 1:4 with linespoints notitle "H2" lw 4 ps 0 lt rgb P4, \
        NaN with linespoints title "H1" lw 4 ps 0 lt rgb PVS, \
        NaN with linespoints title "H2" lw 4 ps 0 lt rgb P4, \
        'table.csv' using 1:3 with linespoints title "H3" dt 2 lw 4 ps 0 lt rgb LOOP, \
        'table.csv' using 1:5 with linespoints title "H4" dt 2 lw 4 ps 0 lt rgb 'green', \
	NaN with lines title "Request" dt 2 lw 4 lt rgb 'black', \
	NaN with lines title "Reconfig" dt 3 lw 4 lt rgb 'gray'

