#!/bin/bash
#
# tester file for histo.c
# testing.sh
#
# executes commands to make sure histo.c works properly
#
# JemelyR, January 2023, cs50, 23W


# alot of numbers under 16
./histo <<EOF
1 6 7 7 7 8 9 9 9 9 4 11 13 14 10 3 2
EOF

# ignores negatives
./histo <<EOF
-8 -56 -8 -4 -9 0 9 7 5 7
EOF

#enter before eof
./histo <<EOF
12 3 5 8 36 9
8 9 
5 6 8 9
EOF

# no inputs
./histo <<EOF
EOF
 
# multiple big numbers
./histo <<EOF
16 78 99 304 35 720 75 120 404 634 345 675 128 345 345 223 56 88 97 99 20 363
EOF

# one huge number
./histo <<EOF
567289
EOF

#alot of same number
./histo <<EOF
2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2
EOF



