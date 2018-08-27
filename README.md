# Cortex

This repository contains the arduino sketches for PLC's used in the cortex escape room. 
The escape game has three rooms to play, each of which has its own directory (dokter, heksen, jagers). 

The files below are shared between the rooms. For lack of better arduino `include` mechanisms, these files are duplicated by a build script. The source files are located in the `lib` directory. 

Building is done using [aap](http://www.a-a-p.org/). Run `aap` to update `*.lib.ino` files. 



#### lib/common.ino

*Purpose:* common functions

*Interfacing:* requires no additional implementation



#### lib/ethernet.ino

*Purpose:* TCP interfacing

*Interfacing:* 

* requires implementation of `handleApiRequest(String apiPath)`
* requires regularly calling of `handleEthernet()` , e.g. in `loop()`

