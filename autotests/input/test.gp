set terminal pdf
plot "data-simple" using 1:2 with lines, \
     "data-simple" using 1:3 with linespoints

# simple plotting
plot '2col.dat'                                # assumes col1=x, col2=y; shows '+' at data points
plot '2col.dat' with lines                     # connect points with a line

# plot a subset of the data
plot[1:5] '2col.dat' with linespoints          # plot the first 5 elements

# add a title to your line
plot '2col.dat' with lines title 'my curve'    # this is really the line-title in the legend

# map the columns to the x- and y-axes
plot '2col.dat' using 1:2                      # 1=x, 2=y (this is the default)
plot '2col.dat' using 2:1                      # 2=x, 1=y (reverse the graph)

# abbreviations
plot '2col.csv' u 1:2 w l title 'Squared'      # 'u' - using, 'w l' - with lines

set title 'Hello, world'                       # plot title
set xlabel 'Time'                              # x-axis label
set ylabel 'Distance'                          # y-axis label

# labels
set label "boiling point" at 10, 212

# key/legend
set key top right
set key box
set key left bottom
set key bmargin
set key 0.01,100

set nokey     # no key

# arrow
set arrow from 1,1 to 5,10

set multiplot                       # multiplot mode (prompt changes to 'multiplot')
set size 1, 0.5

set origin 0.0,0.5
plot sin(x), log(x)

set origin 0.0,0.0
plot sin(x), log(x), cos(x)

unset multiplot                     # exit multiplot mode (prompt changes back to 'gnuplot')

plot sin(x) title 'Sine Function', tan(x) title 'Tangent'

plot sin(x)
plot sin(x)/x

plot 'bp-hr.dat' u 1:2 w lp t 'systolic', 'bp-hr.dat' u 1:3 w lp t 'diastolic', 'bp-hr.dat' u 1:4 w lp t 'heartrate'

set terminal postscript color noenhanced ##setting the term
set output "multiple_files.ps"

set key center ##legend placement

plot [1:5][1:120] \
    for [i = 1:3] "file_".i.".dat" \
    pointsize 1.3 linecolor i+4 \
    title "file\_".i.".dat" \
    with linespoints


file_name(n) = sprintf("file_%d.dat", n)
plot for[i = 1:N] file_name(i) title file_name(i)


# data blocks
$Mydata << EOD
11 22 33 first line of data
44 55 66 second line of data
# comments work just as in a data file
77 88 99
EOD
stats $Mydata using 1:3
plot $Mydata using 1:3 with points, $Mydata using 1:2 with impulses

print '`hostname -s`: '."`hostname -s`"

style1 = "lines lt 4 lw 2"
style2 = "points lt 3 pt 5 ps 2"
range1 = "using 1:3"
range2 = "using 1:5"
plot "foo" @range1 with @style1, "bar" @range2 with @style2

v=0
if (v%2) {
    print "2"
} else if (v%3) {
    print "3"
} else {
    print "fred"
}

v=v+1; if (v%2) print "2" ; else if (v%3) print "3"; else print "fred"

do for [N=1:5] {
    plot func(N, x)
    pause -1
}
