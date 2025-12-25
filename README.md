# Finalmouse Polling Rate Utility

Small tool to list and configure **Finalmouse** USB HID devices.  

---

## Features
- Enumerates connected HID devices  
- Automatically detects Finalmouse devices  
- Sets polling rate: **500 / 1000 / 2000 / 4000 / 8000 Hz**  
- Optional silent mode (no console output)

---

## Usage
```

Usage:
  --help | -h
  --list
  --hz 500|1000|2000|4000|8000 [--vid 0xVVVV --pid 0xPPPP]
  --hz HZ --path "<hid path>"
  --silent | -s

```
### Example
``` 
finalmouse.exe --hz 4000 -s
finalmouse.exe --silent --hz 1000
```
---

## Build
Dependencies:
- hidapi
- base-devel  
- cmake 
- linux kernel with hidraw support
- Optional : usbutils to find device VID/PID if default udev rule doesnt work (device access fail)

---

## Notes
- Recomended to add 99-finalmouse.rules to your udev rules(/etc/udev/rules.d) for proper function
- Tested on Finalmouse ULX Sakura & Frostlord
- Run with sudo if device access fails

---


