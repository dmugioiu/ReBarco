# ReBarco Firmware

Revival of the Barco Strips :-)


## How to use

Select mode using the rotary switch on the controller and plug in.

| Mode   |      Function      |
|----------|:-------------:|
| 0 |  Test Pattern |
| 1 |    Connect to MQTT broker as RGB Light   |
| 2-F | Unused |

### Mode 0
Shows a Test pattern with white LEDs in a brightness gradient.

### Mode 1
Connects to a MQTT Broker over WiFi at ```homeassistant.local``` using the credentials ```barco:barco``` using the homeassistant autoconfiguration protocol.
The device is automatically detected as a color light if homeassistant is set up with the MQTT extension and a broker addon. 
This works by publishing the topic ```homeassistant/light/rebarco_rgb_light/<MAC>/config``` with the following content:
```
{"name": "ReBarco RGB Light <SHORT MAC>","unique_id": "rebarco_rgb_light_<SHORT MAC>","command_topic": "rebarco/<MAC>/set","state_topic": "rebarco/<MAC>/state","schema": "json","rgb": true,"brightness": true}
```

The state of the device is published on the topic ```rebarco/<MAC>/state```:
```
{"state":"ON", "brightness":255, "color":{"r":255,"g":0,"b":174}}
```

The device can be controlled by publishing on the topic ```rebarco/<MAC>/set```:
```
{"state":"ON", "brightness":255, "color":{"r":255,"g":12,"b":36}}
```

**Attention:** If you got your Barco Strip as a gift it probably tries to connect to Dan's WiFi and won't connect to yours.
Entering your WiFi credentials and re-building the firmware is left as an excersise to the reader :-)


### Mode 2 to F
Reserved for future use.

## Ideas for further modes or features
- Improve code architecture and style. Current state was hacked in two evenings.
- Implement the "old" UDP protocol (maybe using WiFi multicast) to support the existing programs. Parts of the rotary switch could be used as an address to use multiple strips.
- Better WiFi provisioning on runtime.
- Zigbee/Thread Light
- Your contribution is highly appreciated :-)

## How to build

- Install VSCode
- Install ESP-IDF Extension
- Follow installation instructions
- Clone repository
- Open Folder in VSCode
- Connect USB (Normally no reset or button press necessary)
- Select COM Port in Footer Bar
- Select JTAG as programming mode in Footer Bar
- Press Flash in Footer Bar
