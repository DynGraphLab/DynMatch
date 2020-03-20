set datafile separator ","
set terminal png size 800,800
set title "Performance Profile Time"
set ylabel '# instances running time  >= fastest algo / t'
set xlabel "t "
set key right bottom 
set grid
set xrange [0:1] reverse

plot "final_output_time" using 1:2 with steps lw 2 lt 1 title 'results_social_neiman', \
"final_output_time" using 3:4 with steps lw 2 lt 3 title 'results_social_dynblossom1', \
