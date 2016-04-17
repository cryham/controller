Crystal Keyboard 3 fork
======================

This is [my fork](https://github.com/cryham/controller/) for the keyboard CK3.

Hardware
--------

The keyboard is made of:
* Teensy 3.1 (or 3.2)
* OLED display: SSD 1306, 128x64 mono
* A4 Tech KX-100 keyboard
* 2 LEDs for layer info

Media
-----

For more info about CK3 read this [forum post](https://geekhack.org/index.php?topic=81312.0), see
[Picture galleries](https://picasaweb.google.com/106514390902066155561), and [Videos](https://www.youtube.com/channel/UC9-gc9xgEeuSSKB8_dESLGw/videos).

Features
--------

The fork of Kiibohd Controller adds these features to it:
* Included teensy3 directory from [Paul's cores repo](https://github.com/PaulStoffregen/cores), reduced
* Support for display with Adafruit GFX and [1306][https://github.com/adafruit/Adafruit_SSD1306] libraries
* Gui Menu on display
* Sequences/Macros viewer and editor
* Quite few demos:
  * Space, Balls, Rain, Fountain
  * Polygons 2D, Polyhedrons 3D
  * Waving 2D CK logo
  * Plasma (old school effect)
* Falling blocks game Sixtis with 9 game presets
  * And Gui with options for all parameters for custom games

Details
-------

The keyboard matrix has 18 columns and 8 rows (uses 26 pins) display uses 4 pins (HW SPI) and LEDs use 2.
For info on pins connections see the files in Scan/CK3, [pins.txt](https://github.com/cryham/controller/blob/master/Scan/CK3/pins.txt) and [matrix.h](https://github.com/cryham/controller/blob/master/Scan/CK3/matrix.h).

Currently (full featured) code uses:
* RAM: 68% (of 64kB)
* Flash: 70% (of 256kB)

See main.cpp for customization defines: KII, DEMOS, GAME. Those allow disabling/enabling features.

A basic Kiibohd build (`set(DebugModule "none"` in CMakeFiles.txt) with OLED and Gui uses:
* RAM: 24%
* Flash: 50%



Kiibohd Controller
==================

For reference, refer to Wiki from the [original repo](https://github.com/kiibohd/controller) of this fork.


Licensing
---------

Licensing is done on a per-file basis. See original repo for more info.
My sources are licensed GPLv3, those are in subdirs: demos, gui, games, Scan/CK3.
