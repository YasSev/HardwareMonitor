#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
// As PID's in Linux can get up to 4194304, and we want
// to track the processes according to their PID's
// --> Choose array size one larger because of our implementation
// --> PID 3 == array [3] (O NOT included!)
#define ARRAY_SIZE 4194305

// copies one array to another
void copyArray(int* sourceArray, int* destinationArray, int size) {
    for (int i = 0; i < size; i++) {
        destinationArray[i] = sourceArray[i];
    }
}

// displays the popup window, which asks the user to ignore, suspend or kill the process, which uses a lot of resources
// gets the PID of the process in question as an input
int displayPopupWindow(int pid) {
// return value of the script, which asks the user the options
int ignoreTime = 0;
    char command[100];
    // call the script and write the result into a text file
    sprintf(command, "bash kill.sh %d > kill.txt", pid);
    int retVal = system(command);

    // WIFEXITED returns the status of the child process; returns true, if it successfully terminated.
    if (WIFEXITED(retVal)) {
            FILE* file = fopen("kill.txt", "r");
            if (file) {
                char buffer[100];
                if (fgets(buffer, sizeof(buffer), file)) {
                // reads the duration / ignore time from the file; which is the first entry in the file, when ignore is reached.
                    ignoreTime = atoi(buffer);
                    // printf("The ignoreTime is: %d\n", ignoreTime);
                }
                fclose(file);
            }
        } else {
        	printf("Script ended abnormally!");
        }
    // Returns programStatus (== ignore time), which is either 0 in case of suspension or killing; else it's the user input "ignore time"     
    return ignoreTime;
}

// contains method to search for an element in an array
int contains(int* array, int* target) {
	for (int i = 0; i < 5; i++) {
		if (array[i] == target) {
			return 1;
		}
	}
	return 0;
}

int main() {
	// Initialization of Arrays; Big array for storing all the "counts" of all processes, corresponding to their PID's
	// small array's for storing the top5 most CPU intensive tasks (old and new for comparing them)
	int *array = malloc(ARRAY_SIZE * sizeof(int));
	int oldTop5[5] = {0,0,0,0,0};
	int newTop5[5] = {0,0,0,0,0};
	// Initialization of other variables
	FILE *topOutput;
	char line[256];
	int pid;
	int threshold;
	int repetitions;
	float percentage;
	
	// For the first startup of the program: Ask the user for the threshold value of CPU percentage
	// and for the number of Repetitions (Seconds) until prompt pops up (bash scripts)
	system("sh scaleThreshold.sh > temp.txt");
	system("sh scaleRep.sh > temp2.txt");
	
	// Open the previously generated file to read the threshold given by the user
	FILE* file = fopen("temp.txt", "r");
    	if (file) {
        	char buffer[100];
        	while (fgets(buffer, sizeof(buffer), file)) {
        		char* temp = buffer;
        		// atoi: converts string to integer
        		threshold = atoi(temp);
            		printf("The threshold is: %d \n", threshold);
        	}
        	fclose(file);
    	}
    	
    	// Open the previously generated file to read the number of repetitons given by the user
    	FILE* file2 = fopen("temp2.txt", "r");
    	if (file2) {
        	char buffer[100];
        	while (fgets(buffer, sizeof(buffer), file2)) {
        		char* temp = buffer;
        		repetitions = atoi(temp);
            		printf("The # of repetitions is: %d \n", repetitions);
        	}
        	fclose(file2);
    	}
	
	// Now this is the main part of our program; this part works in an endless loop and monitors the top 5
	// most CPU-intensive processes, and when a threshold is reached --> Asks the user for an action
	while(1) {
		// Write the current values from the top-command into our output file
		system("sh topWriter.sh");
		
		// Wait for 1 second, to get current data
		sleep(1);
		// open the file with the top 5 processes
		topOutput = fopen("top_output.txt", "r");
		if (topOutput == NULL) {
			// error message; if file couldn't be opened
			system("zenity --error --text='Error occurred: File top_output.txt could not be opened!'");
			return EXIT_FAILURE;
		}
		// read the 6 lines printed into the text file
		for (int i = 0; i < 6; i++) {
			if (i == 0) {
				// First line in the text file is: PID %CPU COMMAND
				// ignore this, as it's not relevant for the calculations; only for debugging purposes
				fgets(line, sizeof(line), topOutput);
				continue;
			} else {
				// Here: We have PID %CPU COMMAND values, separated by a string
				// --> split the string to get the individual values
				fgets(line, sizeof(line), topOutput);
				char * tempString = strtok(line, " ");
				pid = atoi(tempString);
				tempString = strtok(NULL, " ");
				percentage = atof(tempString);
				// if the percentage of CPU usage exceeds the pre-defined threshold:
				// increase the counter at the corresponding entry in the array
				// --> PID 3 == array [3]
				if (percentage > threshold) {
					array[pid]++;
					// printf("Process %d uses %f percent of the CPU! and is at count %d\n", pid, percentage, array[pid]);
				} else {
					// if a process uses less than the threshold again; it's count value gets reset
					// --> process needs to use more than the threshold for # of repetitions times consecutively
					array[pid] = 0;
					// printf("Process %d has been reset!\n", pid);
				}
				// update the entries in the new Top 5
				newTop5[i-1] = pid;
			}
		}
		
		// printf("\n");
		fclose(topOutput);
		
		// To reset processes that were part of the top 5 in the previous iteration, but not in the new one
		// as these processes are not part of the top5-output.txt file anymore, and thus don't get reset in the method abode
		for (int i = 0; i < 5; i++) {
			int item = oldTop5[i];
			if (contains(newTop5, item) == 0) {
				array[item] = 0;	
			}
		}
		
		// copy the array; the new Top5 now becomes the old Top 5 for the next iteration
		copyArray(newTop5, oldTop5, 5);
		
		// In the end: check all entries in the array, where the count exceeds the # of allowed repetitions.
		for (int i = 0; i < ARRAY_SIZE; i++) {
			if (array[i] >= repetitions) {
				// displays the pop-up, which asks the user what to do.
				int programStatus = displayPopupWindow(i);
				
				// PrgoramStatus is always zero; except if the user chose "ignore"
				// In this case: decrement the count for the corresponding ignore-time
				array[i] = array[i] - programStatus;
			}
		}
	}
	
	// Free all used memory again.
	free(array);
	free(oldTop5);
	free(newTop5);
	
    system("echo fully finished");
    return 0;
}

