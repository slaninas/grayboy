# GrayBoy

A Game Boy emulator written in C++.

## Goal
The goal of this project is to learn how to write an emulator and get to know the Game Boy architecture.

This project does not aim to produce accurate emulation, nor does it aim to present complete Game Boy emulator (there is missing sound, saves, MBC2).

This may change in the future but with the v0.1 release I consider this project to be done.


## Accuracy
- All CPU instructions are implemented and all blargg individual cpu_instr tests [ROMs](https://github.com/retrio/gb-test-roms/tree/master/cpu_instrs/individual) are passing

- Blargg timing tests fail due to inaccuracies (timer/display is updated after each instruction is executed, not after each read/write operation)

- DMA transfer is instant

- Pixel processing unit is simplified

## Games tested
Most games I tried run fine, if there were issues it were only minor graphics glitches.

Some examples of tested games:

| Tetris |  |
| ------------- | ------------- |
<img src="https://github.com/slaninas/grayboy/blob/master/screenshots/tetris.png?raw=true" width="350"> | <img src="https://github.com/slaninas/grayboy/blob/master/screenshots/tetris2.png?raw=true" width="350">

| Super Mario Land |  |
| ------------- | ------------- |
<img src="https://github.com/slaninas/grayboy/blob/master/screenshots/mario.png?raw=true" width="350"> | <img src="https://github.com/slaninas/grayboy/blob/master/screenshots/mario2.png?raw=true" width="350">

| The Legend of Zelda: Link's Awakening |  |
| ------------- | ------------- |
<img src="https://github.com/slaninas/grayboy/blob/master/screenshots/zelda.png?raw=true" width="350"> | <img src="https://github.com/slaninas/grayboy/blob/master/screenshots/zelda2.png?raw=true" width="350">



## How to build and play on Linux
- Requires compiler with C++20 support and libsdl2-dev installed

```
git clone https://github.com/slaninas/grayboy/
cd grayboy && git submodule --update --init
mkdir build && cd build && make
./src/grayboy your_game
```

## Controls
- Arrow keys, ENTER, SPACEBAR, A, S

## Useful sources
- [opcodes](https://meganesulli.com/generate-gb-opcodes/)
- [emudev](https://emudev.de/gameboy-emulator/overview/)
- [codeslinger](http://www.codeslinger.co.uk/pages/projects/gameboy.html)
- [GbdevWiki](https://gbdev.gg8.se/wiki/articles/Main_Page)
- [BGB](https://bgb.bircd.org/)
- [binjgb](https://github.com/binji/binjgb)
