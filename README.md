Problem Definition

AirAware is designed to estimate the level of nearby activity without requiring external sensors, cloud services, or additional hardware. It uses the ESP32-C3's ability to detect nearby Wi-Fi access points and BLE devices, combining device count and RSSI variation as an indirect proxy for movement and occupancy. The chip's internal temperature sensor is also monitored as a basic system-health indicator. Results are classified into Quiet, Present, or Active and exposed through a local web dashboard.

Wi-Fi/BLE scan results and chip temperature are sensed periodically, classified into an ambient state, and reflected through the onboard LED and local dashboard — a full sense → decide → act loop with zero external hardware.

The firmware was built and iterated through FirmGen, using its prompt → plan → deploy (build, flash, monitor) → evidence cycle rather than hand-written ESP-IDF code, with each module (Wi-Fi scanning, BLE scanning, classification, temperature monitoring, web server, LED control) generated and verified as a separate, independently testable increment.

Target Users
Embedded systems students learning ESP32-C3, Wi-Fi, BLE, and modular firmware architecture.
IoT developers prototyping sensorless occupancy/activity estimation.
Researchers and hobbyists experimenting with RF-based environmental sensing.
Labs and classrooms where a simple local indication of changing activity is useful.
Firmware developers interested in non-blocking periodic tasks, state machines, scanning, and local web interfaces.
Build Instructions
Use an ESP32-C3 development board with Wi-Fi, BLE, and an onboard LED.
Set up the ESP32 toolchain through FirmGen's toolchain manager (backed by ESP-IDF) — FirmGen detects what's installed and guides you through anything missing.
Create the AirAware project in FirmGen with separate modules for:
Configuration
Wi-Fi scanning
BLE scanning
Classification
Internal temperature monitoring
Web server
LED/status control
Enter the desired AP name and password in the configuration module.
Configure scan intervals, device-count thresholds, RSSI-variance thresholds, and state-transition cooldowns in the same module.
Deploy: build, flash, and monitor through FirmGen's bounded deploy cycle.
Open the serial monitor to observe scan results, failures, temperature readings, and state transitions.
Connect a phone or laptop to the ESP32-C3's Wi-Fi access point.
Open the ESP32's local IP address in a browser to view the dashboard.
Observe the onboard LED as it changes according to the current ambient state.

The firmware uses independent FreeRTOS tasks/timers and non-blocking module interfaces so Wi-Fi scanning, BLE scanning, classification, temperature monitoring, and the web server do not unnecessarily block one another.

Wiring / BOM

No external wiring is required.

Component	                Quantity	         Purpose
ESP32-C3 development board	 1	               Sensing, processing, Wi-Fi/BLE, web server, and control platform
USB-C/Micro-USB cable	    1	               Power and firmware flashing
Computer	                   1	               Development and flashing
Phone/laptop	             1	               Optional client for viewing the local dashboard

The ESP32-C3's Wi-Fi radio, BLE radio, internal temperature sensor, CPU, and onboard LED provide all functionality.

Limitations
Device detection is not occupancy detection. A person carrying no detectable Wi-Fi/BLE device may not appear in the measurements.
Modern phones may randomize MAC addresses, making individual-device tracking unreliable.
Wi-Fi/BLE scan results depend heavily on radio conditions, transmit power, device behavior, walls, interference, and antenna orientation.
RSSI is noisy and does not directly represent distance or movement.
Device count can represent nearby electronics rather than people.
The internal temperature sensor is primarily useful as a relative system-health signal, not as an accurate ambient-temperature sensor.
Simultaneous Wi-Fi and BLE activity is constrained by the ESP32-C3's shared 2.4 GHz radio resources.
Scanning periodically rather than continuously means very short-lived devices may be missed.
Thresholds will need calibration for the particular environment.
The local dashboard is only accessible while connected to the ESP32-C3's access point/network.
No cloud storage, remote access, historical database, or external broker is provided.
The system should be treated as an experimental activity/occupancy proxy, not a security, safety, or people-counting system.
