# Cortex

This repository contains the arduino sketches for PLC's used in the cortex escape room. 
The escape game has three rooms to play, each of which has its own directory (dokter, heksen, jagers). 

The files below are shared between the rooms. For lack of better arduino `include` mechanisms, these files are duplicated. (A build script needs to be added to fix this)



#### common.ino

*common functions*

*Interfacing* requires no additional implementation



#### ethernet.ino

*enables HTTP interfacing*

*Interfacing* requires implementation of `handleApiRequest(String apiPath)`

