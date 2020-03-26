set datafile separator ","
set terminal postscript enhanced color 
set output '| ps2pdf - output.pdf'
set title "Performance Profile Quality"
set ylabel '# instances >= t opt'
set xlabel "t "
set key right bottom 
set grid
set xrange [0.9:1] reverse

plot "final_output" using 1:2 with steps lw 4 lt 1 title 'results_dynprelim_randomwalk0.1', \
"final_output" using 3:4 with steps lw 4 lt 3 title 'results_dynprelim_randomwalkdegreesettle0.1', \
"final_output" using 5:6 with steps lw 4 lt 5 title 'results_dynprelim_randomwalkdegreesettle_10lowdegree0.1', \
"final_output" using 7:8 with steps lw 4 lt 7 title 'results_dynprelim_dynblossom_speed_0.1', \
"final_output" using 9:10 with steps lw 4 lt 9 title 'results_dynprelim_dynblossom_speed_0.25', \
