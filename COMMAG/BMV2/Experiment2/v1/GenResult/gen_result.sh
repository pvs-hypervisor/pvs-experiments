set -xe

tar xjvf Logs.tar.bz2

g++ -std=c++11 -O3 -o gen_table gen_table.cpp
./gen_table Logs/
gnuplot plot.plot
epstopdf plot.eps

rm plot.eps
rm gen_table
rm table.csv
rm -r Logs/
