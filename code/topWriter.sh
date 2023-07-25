#!/bin/bash/
 
# args=$#

# Writes the current top command status into a file
# only prints the top 5 most CPU-intensive tasks, and their PID and command name
top -b -n 2 -d 0.1 | awk 'NR>6 && NR <=12 {print $1, $9, $12}' > top_output.txt
