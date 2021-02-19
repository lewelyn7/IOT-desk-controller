# IOT Arduino desk controller

Software for Arduino desk controller. Main purpose of device is to manage state of lights, switch color and effects on RGB LED strip and print messages on two screens. 

 #### Output devices:
 * WS2812b ambilight LED strip
 * 4-digit 7-segment LED display
 * 2x16 LCD display
 * Desk light bulb via relay
 * can be extended too many more since there are free pins left on microcontroller and port expander

#### Device has 3 user interfaces:
 * MQTT commands
 * RS-232 Serial interface for direct commands from PC
 * DIY hardware panel with switches and encoders

#### Feautres:
 * switch different effects on LED strip
 * control state of desk light via relay
 * Integration and control of other Smart Home IOT devices via MQTT protocol
 * Handle notifications/message sent from PC or Smart Home
 * control mute/unmute state of PC microphone. Device shows appropriate message on screen and red light on LED strip ( sort of "ON AIR" sign :)
 * show current time on LED screen
 * React and perform different actions whether PC is about to sleep or wake up. This is done with [Python script and D-BUS](https://github.com/lewelyn7/python-macro-keyboard)


## Hardware

Device is built on NodeMCU board with ESP8266 microcontroller, which communicates with other peripherals via I2C, 1-Wire, RS-232 and 802.11 b/g/n Wi-Fi

Main Hardware used:
 * NodeMCU ESP8266 board
 * DS3231 Real-time clock
 * PCF8574 port expander
 * 4-digit LED screen with TM1637 driver
 * WS2812b RGB LED strip
 * Control Panel:
    * 6x LEDs
    * 8x switches
    * 4x rotary encoders
    * Arduino Pro micro
    * MCP23S17 port expander
 * relay module with appropriate current protection


## Software

Software is written in C++ and uses Arduino libraries to provide abstraction layer for hardware manipulation. Led strip control is done with [FastLED library](https://github.com/FastLED/FastLED). Every hardware module like LCD screen or RTC clock is modelled as class that provides methods and specific functionality to other modules. Internally, classes use other libraries from Arduino Library Manager to communicate with hardware directly.

To provide multiple features and functionalities protothreading mechanism is implemented using [Arduino Thread](https://github.com/ivanseidel/ArduinoThread) library.

Communication with PC uses RS-232 and is handled by [Python script](https://github.com/lewelyn7/python-macro-keyboard) on computer side.