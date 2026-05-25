**This is the first iteration of Bluetooth Low Energy (BLE) Scanner using only a ESP32 WROOM 32E built in ESP-IDF v6.0 using C++ and FreeRTOS.**

In this project the ESP32 intercepts raw BLE signals and captures the following key metrics:
1) Timestap
2) MAC Address
3) RSSI (Received Signal Strength Indicator)


Some key features of the code are as follows:
1) Dual Core use: The heavy NimBLE radio stack operates asynchronously, offloading parsed packet envelopes into a FreeRTOS Queue.

2) 100% Scanning Duty Cycle: Configured with matching scan windows and intervals to eliminate receiver downtime.

Key components:
1) MCU: ESP32 WROOM 32E
2) Framework: ESP-IDF v6.0.1 (Native CMake Build Toolchain)
3) Language: C++ / C Standard Library
4) OS core: FreeRTOS Kernel