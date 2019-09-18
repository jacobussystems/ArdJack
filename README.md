# ArdJack

A command interpreter for Arduinos, receiving the user's commands via e.g. a serial or UDP connection.

MIT license.


## Main Features

* ArdJack works on `Arduino MKR1010`, `Adafruit Feather M0`, `DEVKIT ESP32`, `SparkFun RedBoard Turbo`, `Arduino Due` (no WiFi), and potentially any Arduino IDE-compatible board with at least 32 KB of SRAM. (N.B. Some combinations of active features may overflow memory on 32KB boards due to memory size and/or heap fragmentation.)
* Includes support for WiFi, UDP, TCP, HTTP.
* Object-oriented approach – there are user commands to create user objects: `Devices`, `Connections`, `Beacons`, `Bridges` etc., and to configure and use them. (Each has a corresponding C++ class.)
* A Device, e.g. the board running the code, has a `Part` for each input or output. The `ArduinoDevice` class includes setup for several boards and may be easily extended.
* A Device may be configured to use a `Shield` (see example class `ArduinoMFShield`), which then imposes its own Part configuration.
* A Device may have `UserParts` to cater for connected components, e.g. sensors, actuators and displays. The `UserPart` class is subclassed for each type - see the example classes for DHT11/DHT22 sensors and NeoPixel displays.
* Adapts to the current board type when compiled - see file `DetectBoard.h`.
* Configurable - see the #defines at the top of file `Globals.h`.
* Flexible routing scheme for routing input from Connections.
* Macros – each macro stores one or more user commands.


## Example ArdJack Commands

Command | Description
--- | ---
`configure udp0 outip=192.168.1.222 outport=6001` | The application creates a UDP Connection called `udp0` by default. This command configures `udp0`'s output IP address to `192.168.1.222` and changes its output port from the default `5001` to `6001`.
`add beacon beacon0` | Adds a Beacon called `beacon0`.
`configure beacon0 target=udp0 "text=Hi from xx" interval=1000` | Configures `beacon0` to send text via Connection `udp0` every second.
`activate beacon0` | Applies the configuration and makes `beacon0` active.
`configure beacon0 interval=500` | Modifies `beacon0`’s configuration. If it was active, the command first deactivates the Beacon. After modifying the configuration, it is reactivated.

NOTE: Each command above is one line.

See the Wiki for more detail.

The code is developed using Visual Studio 2019 with Visual Micro.


## ArdJackW

ArdJackW is a Windows console application based on ArdJack, also developed in Visual Studio 2019.

The source files are in the `src` folder, and there is an associated Unit Test project in subfolder `UnitTest1`.

