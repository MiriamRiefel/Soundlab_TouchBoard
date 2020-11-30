# Soundlab_TouchBoard
Arduino code used for Touch Board control 

code created for Soundlab Enschede, components Drumdorp and Groovelaan.
by Miriam Riefel

chunks of code taken and adjusted from Bare Conductive multi-board-touch-midi
https://github.com/BareConductive/multi-board-touch-midi

chunks of code taken and adjusted from Adafruit Neopixel Strandtest
https://github.com/adafruit/Adafruit_NeoPixel/blob/master/examples/strandtest/strandtest.ino

This code is for: 
- using MULTIPLE touch boards 
- to input KEY surrogates (the electrodes will trigger keys just like a computer keyboard would)
- and trigger DIGITAL LEDSTRIPS in divided sub-strips with specified colours per sub-strip

Upload the relevant primary board script to the primary board. 
The secondary board script is the same for both components. 
The exact same secondary board script could be uploaded to any additional secondary boards (make sure to adjust the primary board script as stated below).

If you want to: 
- change the total number of Touch Boards (now 2) 
- change the amount of sub-LEDstrips (now 4)
- change the amount of LEDs per sub-strip (now varying sizes) 
- or change the colours of sub-strips (now red, green, tule & blue) 
you can do so by searching 'ADJUST' inside the scripts. There it will be stated when to adjust, what to adjust and in which line this should be done.


Make sure to connect the Touch Boards, LED strips and power supply according to the schematics which can be found in the github links above. 
