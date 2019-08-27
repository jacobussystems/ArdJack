# IoTJackAC1

It’s essentially 'yet another' command interpreter, receiving commands via e.g. a serial or UDP connection.
It has a liberal MIT license – essentially, any code can be reused by anyone for any purpose.

<h2>Main Features</h2>

* Works on Arduino MKR1010, Adafruit Feather M0, DEVKIT ESP32, SparkFun RedBoard Turbo, Arduino Due (no WiFi),  and potentially any Arduino-compatible board with at least 32 KB of SRAM.
* Includes support for WiFi, UDP, TCP, HTTP.
* Object-oriented approach – create Devices, Connections, Beacons, Bridges etc., configure and use them.
* Macros – each macro stores one or more commands on a line.

<h2>Example commands</h2>

	configure udp0 outip=192.168.1.222 outport=6001
A UDP Connection called ‘udp0’ is created by default.  This command configures the output IP address to 192.168.1.222 and changes the output port from the default 5001 to 6001.

	add beacon beacon0
Adds a Beacon called ‘beacon0’.

	configure beacon0 target=udp0 “test= Hi from xx” interval=1000
Configures the Beacon to send text via Connection ‘udp0’ every second.

	activate beacon0
Applies the configuration and makes the Beacon active.

	configure beacon0 interval=500
Modifies the Beacon’s configuration. (If it was active, the command first deactivates the Beacon. After modifying the configuration, it is reactivated.)


See the Wiki for more detail.
The code was developed using Visual Studio 2019 with Visual Micro.

