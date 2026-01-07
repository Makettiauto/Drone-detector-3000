# Drone-Detector-3000
A drone detector project built using the AD8317 demodulating logarithmic amplifier and the ESP32 microcontroller.

# Context
Due to the rapid evolution of modern warfare, personal drone detection has become essential. The primary objective of this device is to provide the soldier with enough warning time to seek shelter or mount a defense. The detector does not need to fully demodulate the signal; instead, it detects RF emitters that exhibit characteristics resembling a drone signal and alerts the user immediately.

# My Solution
This solution utilizes the AD8317 demodulating logarithmic amplifier and an ESP32 microcontroller, alongside an OLED display, a buzzer, and status LEDs.

The device performs two main functions:

Signal Monitoring: It displays the aggregate signal strength (in dBm) within the 1 MHz – 10 GHz frequency range.

Drone Identification: It specifically warns the user if a drone signature is detected. This is achieved by analyzing the average signal strength, the standard deviation of signal peaks, and the Peak-to-Average Ratio (PAR).
