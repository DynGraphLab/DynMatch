set datafile separator ","
set terminal png size 800,800
set title "Performance Profile Quality"
set ylabel '# instances >= t opt'
set xlabel "t "
set key right bottom 
set grid
set xrange [0.9:1] reverse

plot "final_output" using 1:2 with steps lw 2 lt 1 title 'results_social_neiman', \
"final_output" using 3:4 with steps lw 2 lt 3 title 'results_social_dynblossom0.5', \
