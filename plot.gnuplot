set datafile separator ","
set terminal png size 400,400
set title "Performance Profile"
set ylabel ""
set key right bottom 
set grid
set xrange [0.7:1] reverse
plot "final_output" using 1:2 with steps lw 2 lt 3 title 'Performance Profile', \
      "final_output" using 11:12 with steps lw 2 lt 3 title 'Performance Profile'

