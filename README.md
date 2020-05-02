# Arduventure ![](sprites/sword.png) ![](sprites/ghost.png) ![](sprites/key.png)
A game that plays on an Arduino Mega 2560 using no external libraries other than the standard AVR/Arduino ones.
Using my [Arduino-VGA](https://github.com/byteduck/Arduino-VGA) code, I decided to make a little Atari 2600 "Adventure" -esque game.
Check [Arduventure.cpp](src/Arduventure.cpp) and [Arduventure.h](include/Arduventure.h) for which pins to hook everything to.

## Building
This project uses PlatformIO to build, although if you threw all of the files from include and src into the same folder and renamed `Arduventure.cpp` to `Arduventure.ino` it would probably work fine with the regular Arduino IDE.