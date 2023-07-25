#!/bin/bash

# Displays the popup window, where users can choose the duration / number of iterations; between 5 - 180 seconds; default is 30
VALUE=$(zenity --scale --text="Adjust number of seconds until notification" --title='Duration' --min-value='5' --max-value='180' --value='30')

case $? in
    0)
    # returns chosen value, if OK is pressed
        echo $VALUE
        ;;
    1)
    # returns default, if cancel is pressed
        echo 30
        ;;
esac
