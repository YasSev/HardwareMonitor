# HardwareMonitor
OSProject

This project was part of the bachelors course "Operating Systems" at the University of Basel.
The goal was to create a HardwareMonitor, which not only passively monitors the CPU resources, but also takes a more active role.
We designed and implemented a Hardware-/ ActivityMonitor for Linux, where the user can choose a threshold of CPU utilization and a duration in seconds, and then our program continuously monitors all processes in the background and if a process surpasses that threshold and duration; the user gets presented a pop-up window, which gives him the option to either terminate the process, suspend it, or ignore the warning (if the process should continue) for a certain amount of time.
More details can be viewed in the report, as well as in the user-manual, in the doc-folder.
