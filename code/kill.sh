#!/bin/bash

pid="$1"

# get the process name via the ps command and it's pid, which is given as an argument to the script in the "displayPopupWindow" method.
process_name=$(ps -p "$pid" -o comm=)

# get the corresponding button, which the user pressed: ignore, suspend, or kill process
response=$(zenity --info --text="Process '${process_name}' with PID: ${pid} uses a lot of resources!" --title='Choose next action:' --ok-label='Ignore' --extra-button='Suspend Process' --extra-button='Kill Process')

case $? in
    0)
        # Make a new pop-up window and ask the user for ignore-duration
                duration=$(zenity --scale --text="Choose the duration of ignoring [s]" --title='Duration' --min-value='10' --max-value='300' --value='60')
                # this gets printed to a file; so if ignore is chosen, the first entry in the file is the ignore-duration,
                # which is then read by the main program and further processed
                echo "$duration"
		case $? in
		    0)
		    # if a value was chosen
			echo "Process with name ${process_name} and PID $pid ignored for $duration seconds..."
			;;
		    1)
		    # If cancel-button was pressed
			echo "Process not ignored"
			;;
		esac
                ;;
    1)
    	# if one of the extra- buttons was pressed
        case "$response" in
        # if suspend-button was pressed
            "Suspend Process")
            # Make pop-up window and ask the user for suspend-duration
                duration=$(zenity --scale --text="Choose the duration of suspension [s]" --title='Duration' --min-value='10' --max-value='300' --value='60')
		case $? in
		    0)
			(
                            kill -STOP "$pid"
                            echo "Process with name ${process_name} and PID $pid suspended for $duration seconds..."
                            sleep "$duration"
                            # Resume the process
                            kill -CONT "$pid"
                        ) & # is allowing this operation to be made in the background; while the script still continues running
                        # so the process gets suspended for a certain duration, while the rest of the program continues
			;;
		    1)
			echo "Process not suspended"
			;;
		esac
                ;;
            "Kill Process")
            # if the kill-button was pressed; we kill the corresponding process
            	echo "Process with name ${process_name} and PID $pid killed..."
                kill ${pid}
                ;;
        esac
        ;;
esac
