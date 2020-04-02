set datafile separator ","
set terminal postscript enhanced color 
set output '| ps2pdf - output.pdf'
set title "Performance Profile Quality"
set ylabel '# instances >= t opt'
set xlabel "t "
set key right bottom 
set grid
set xrange [0.9:1] reverse

plot "final_output" using 1:2 with steps lw 4 lt 1 title 'results_dynprelim_blossom', \
"final_output" using 3:4 with steps lw 4 lt 3 title 'results_dynprelim_neiman', \
