# Drone-detector-3000

A drone detector build using the AD8317 demodulating logaritmic amplifier and the ESP32 microcontroller.

# Context

Every modern soilder should have a drone detector due to the rapid change in warfare. The only task of this kind of detecor is to give the soilder time to seek shelter or mount a defence against the danger. The detector does not need to demodulate the signal. Instead it only needs to detect a emitter that somewhat resembles a drone signal and infrom the soilder about it.

# My solution

My solution uses the the AD8317 demodulating logaritmic amplifier and the ESP32 microcontroller along side a OLED display, buzzer and leds. The detector informs the user of the summed strength of the signals in the 1MHz - 10GHz frequency range in the unit dBm. It also warns the user if speficily a drone signal is detected by calculating the signal strength average, standard deviation of the signal peaks and the peak to average ratio.
