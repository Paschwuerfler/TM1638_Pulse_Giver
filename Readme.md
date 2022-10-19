# Pulse Giver

> **Note**
> If you want to copy/paste the code into Arduino, go [here](/src/main.cpp)

This code uses: https://github.com/rjbatista/tm1638-library

A simple pulse generator based on the TM1638 Led+Key module. This was written in a very short amount of time, so dont expect the code to be nice. 

Functionality: 

The first two digits show the number of pulses to be sent. 

The next two digits show the off_time in seconds (pay attention to the decimal marker!)

The last two digits show the on-time. 


Buttons 1/2; 4/5; 7/8 are -/+ buttons to adjust the respective pulse parameters shown on the display. 

Button 3 saves the current configuration (not implemented yet). 

Button 4 sends the pulse, during pulsing, the output state is indicated by LED_BUILTIN. The display is not active to maintain timing accuracy. 

