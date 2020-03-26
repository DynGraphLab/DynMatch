set datafile separator ","
set terminal postscript enhanced color 
set output '| ps2pdf - output.pdf'
set title "Performance Profile Quality"
set ylabel '# instances >= t opt'
set xlabel "t "
set key right bottom 
set grid
set xrange [0.9:1] reverse

