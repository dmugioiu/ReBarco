# ReBarco Firmware

Revival of the Barco Strips :-)


## How to use

Select mode using the rotary switch on the controller and plug in.

| Mode   |      Function      |
|----------|:-------------:|
| 0 |  Test Pattern |
| 1 |    Connect to MQTT broker   |
| 2-F | Unused |

### Mode 1
Shows a Test pattern with white LEDs in a brightness gradient.

### Mode 2
Connects to a MQTT Broker over WiFi at "homeassistant.local" using the credentials "barco:barco" using the homeassistant autoconfiguration protocol.
The device is automatically detected as a color light if homeassistant is set up with the MQTT extension and a broker addon.

**Attention:** If you got your Barco Strip as a gift it probably tries to connect to Dan's WiFi and won't connect to yours.
Entering your WiFi credentials and re-building the firmware is left as an excersise to the reader :-)


### Mode 2-F
Reserved for future use.

## Ideas for further modes or features
- Implement the "old" protocol, maybe using WiFi multicast to support the existing programs.
- Better WiFi provisioning without rebuilding.
- Your contribution is highly appreciated :-)

## How to build

- Install VSCode
- Install ESP-IDF Extension
- Follow installation instructions
- Clone repository
- Open Folder in VSCode
- Connect USB (Normally no reset or button press neccesary)
- Select COM Port in Footer Bar
- Select JTAG as programming mode in Footer Bar
- Press Flash in Footer Bar
