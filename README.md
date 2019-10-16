# QtArduSim

Electronic Circuit Simulator


QtArduSim is a simple real time electronic circuit simulator.

It's a general purpose electronics and microcontroller simulation supporting AVR and Arduino.

AVR simulation is provided by simavr.

This is not an accurate simulator for circuit analisis, it aims to be the fast, simple and easy to use.
This means simple and not very accurate electronic models and limited features.

Intended for hobbyist and students that want to learn and experiment with simple circuits.
The objective is to intergate QtArduSim with VS-Code/Platformio:

 - The code is created in VS-Code/Platformio and loaded via the virtual serial port,
 - The circuit is created in QtArduSim,
 - Debugging is done via GDB via the Platfromio integration.

## Building QtArduSim:

Build dependencies:

 - Qt5 dev packages
 - Qt5Core
 - Qt5Gui
 - Qt5Xml
 - Qt5Widgets
 - Qt5Concurrent
 - Qt5svg dev
 - Qt5 Multimedia dev
 - Qt5 Serialport dev
 - Qt5 Script dev
 - Qt5 qmake
 - libelf dev
 - gcc-avr
 - avr-libc
 
(This code is beeing tested on  Ubuntu 16.04)
 
Once installed go to build_XX folder, then:

```
$ qmake
$ make
```

In folder build_XX/release/QtArduSim_x.x.x you will find executable and all files needed to run QtArduSim.


## Running QtArduSim:

Run time dependencies:

 - Qt5Core
 - Qt5Gui
 - Qt5Xml
 - Qt5svg
 - Qt5Widgets
 - Qt5Concurrent
 - Qt5 Multimedia
 - Qt5 Multimedia Plugins
 - Qt5 Serialport
 - Qt5 Script
 - libelf


QtArduSim executable is in bin folder.
No need for installation, place QtArduSim folder wherever you want and run the executable.
