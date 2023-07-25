#!/bin/bash

# Displays the pop-up window, where users can choose the threshold between 50 and 100 % of CPU usage; default: 80
VALUE=$(zenity --scale --text="Choose the max. % of CPU utilization" --title='Threshold' --min-value='50' --max-value='100' --value='80')

case $? in
    0)
    # If OK is pressed: return chosen value
        echo $VALUE
        ;;
    1)
    # If cancel is pressed: return default of 80
        echo 80
        ;;
esac
