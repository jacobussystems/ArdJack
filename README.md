# IoTJackAC1

It’s essentially 'yet another' command interpreter, receiving commands via e.g. a serial or UDP connection.
It has a liberal MIT license – essentially, any code can be reused by anyone for any purpose.


## Main Features

* Works on Arduino MKR1010, Adafruit Feather M0, DEVKIT ESP32, SparkFun RedBoard Turbo, Arduino Due (no WiFi), and potentially any Arduino-compatible board with at least 32 KB of SRAM. (N.B. Some combinations of active features may overflow memory on 32KB devices.)
* Includes support for WiFi, UDP, TCP, HTTP.
* Object-oriented approach – commands to create user objects: Devices, Connections, Beacons, Bridges etc., and to configure and use them. (Each has a corresponding C++ class.)
* A Device, e.g. the board running the code, has a 'Part' for each input or output.
* A Device may be configured to use a 'Shield' (see example class *ArduinoMFShield*).
* A User Part class may be extended to add special parts (see example classes for DHT11/DHT22 sensors and NeoPixel display).
* Adapts to the current board type when compiled - see file *DetectBoard.h*.
* Configurable - using #defines in file *Globals.h*.
* Macros – each macro stores one or more commands on a line.


## Example Commands

	configure udp0 outip=192.168.1.222 outport=6001
A UDP Connection called ‘udp0’ is created by default. This command configures the output IP address to 192.168.1.222 and changes the output port from the default 5001 to 6001.

	add beacon beacon0
Adds a Beacon called ‘beacon0’.

	configure beacon0 target=udp0 "text=Hi from xx" interval=1000
Configures the Beacon to send text via Connection *udp0* every second.

	activate beacon0
Applies the configuration and makes the Beacon active.

	configure beacon0 interval=500
Modifies the Beacon’s configuration. (If it was active, the command first deactivates the Beacon. After modifying the configuration, it is reactivated.)


See the Wiki for more detail.
The code was developed using Visual Studio 2019 with Visual Micro.

