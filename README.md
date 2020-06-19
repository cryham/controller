Crystal Keyboard 3 and 4
========================

This is [my fork](https://github.com/cryham/controller/) for keyboards CK3 and CK4.

More info about them on [My Website](http://cryham.tuxfamily.org/portfolio/crystal-keyboard-3-and-4), this [Forum post](https://geekhack.org/index.php?topic=81312.0), in
[Picture galleries](https://photos.google.com/share/AF1QipOSiTtXabEp8hJKx9ejlzOQst0iujq8XjUbXBfdKm8a0mzGJq3DEYh4QgFq9Et07Q?key=SThSS2dkUVVyMDBnem05TzVhN2lXcUxKZlRJVUdR) with descriptions, and [Videos](https://www.youtube.com/channel/UC9-gc9xgEeuSSKB8_dESLGw/videos).

Hardware
--------

The keyboard is made of:
* Teensy 3.1 or 3.2
* OLED display: SSD 1306, 128x64 mono
* A4 Tech KX-100 keyboard for CK3, but any other could be used
* 2 LEDs for layer info

Features
--------

The fork of Kiibohd Controller adds these features to it:
* Included teensy3 directory from [Paul's cores repo](https://github.com/PaulStoffregen/cores), reduced
* Support for display with [Adafruit GFX](https://github.com/adafruit/Adafruit-GFX-Library) and [SSD 1306](https://github.com/adafruit/Adafruit_SSD1306) libraries
* Gui Menu
* Sequences/Macros viewer and editor
  * Useful for e.g. passwords, frequently typed words (no need to type anymore)
  * And often used key combinations (macros)
  * Loaded and saved in eeprom
* Mouse move with acceleration
* Help pages listing all key shortcuts used
* Status pages showing
  * Active layers, protocol
  * Pressed keyes, modifiers, locks
  * When ghosting: columns and rows count
* Quite few [demos](https://www.youtube.com/watch?v=66CksiS55fg):
  * Space, Balls, Rain, Fountain
  * Polygons 2D, Polyhedrons 3D
  * Waving 2D CK logo
  * Plasma (old school effect)
* Falling blocks [game](https://www.youtube.com/watch?v=0s5GEGBwL94) Sixtis with 9 game presets
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

For reference, read Wiki from the [original repo](https://github.com/kiibohd/controller) of this fork.


Licensing
---------

Licensing is done on a per-file basis. See original repo for more info.
My sources are licensed GPLv3, those are in subdirs: demos, gui, games, Scan/CK3.
