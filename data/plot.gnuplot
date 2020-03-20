set datafile separator ","
set terminal png size 400,400
set title "Performance Profile"
set ylabel '# instances >= $\tau$ opt'
set xlabel " $\tau$ "
set key right bottom 
set grid
set xrange [0.7:1] reverse
plot "final_output" using 1:2 with steps lw 2 lt 5 title 'Baswana', \
     "final_output" using 11:12 with steps lw 2 lt 3 title 'DynBlossom0.5', \
     "final_output" using 9:10 with steps lw 2 lt 4 title 'DynBlossom 0.25', \
     "final_output" using 17:18 with steps lw 2 lt 6 title 'Neiman', \

