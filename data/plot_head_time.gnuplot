set datafile separator ","
set terminal postscript enhanced color 
set output '| ps2pdf - output_time.pdf'
set title "Performance Profile Time"
set ylabel '# instances running time  >= fastest algo / t'
set xlabel "t "
set key right bottom 
set grid
set xrange [0:1] reverse

