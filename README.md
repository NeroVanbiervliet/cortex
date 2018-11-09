# Cortex

This repository contains the arduino sketches for PLC's used in the cortex escape room. 
The escape game has three rooms to play, each of which has its own directory (dokter, heksen, jagers). 



## Dependencies

* Install through arduino library manager
  * Controllino
  * TimerOne
  * MsTimer2
* Download zip, extract, rename: [HX711](https://github.com/bogde/HX711)
* Install through board mangager: [Controllino boards](https://github.com/CONTROLLINO-PLC/CONTROLLINO_Library#installation-guide)



## Configuration

Configuration is done using the `config.aap` file, which is loaded during building. An example configuration file is provided: `config.example.aap`. **Warning:** building will fail without config file. 

Variables to be set include server ip addresses and mac addresses for the devices. 



## Building

The files below are shared between the rooms. For lack of better arduino `include` mechanisms, these files are duplicated by a build script. The source files are located in the `lib` directory. 

Building is done using [aap](http://www.a-a-p.org/). Run `aap` to update `*.lib.ino` files. 



## Libraries

Libraries define shared code between different rooms. Most of them require calling a `setup` function during the `setup()` phase in the main code file. Some require additional interfacing. 



#### lib/common.ino

*Purpose:* common functions

*Interfacing:* requires no additional implementation



#### lib/ethernet.ino

*Purpose:* TCP interfacing

*Interfacing:* 

* requires implementation of `handleApiRequest(String apiPath)`
* requires regularly calling of `handleEthernet()` , e.g. in `loop()`



#### lib/storm.ino

*Purpose:* performing a storm using a combination of light flashing and sound

*Interfacing:* 

* requires a `#define` macro for `RELAIS_STROBOSCOPE` (relais pin of stroboscope)

