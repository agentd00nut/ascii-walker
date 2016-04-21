# ascii-walker

how 2 readme?

This is a C based drunken walk that wraps around the edges of the terminal screen.
It does not support screen resizing while running but between runs thats fine.

It runs untill you kill it with ctrl-c.


# Some neat things

- When the walker steps it cycles that tile through 4 different tiles... Stepping on a fourth tile resets it.
- There are colors!
- There will be a debug mode to walk around yourself, it's like an etch a sketch.
- There will be a comendium of half broken versions that actually looked cool.

# Future Things

- Ability to feed in an image and have it drunken walk around slowly revealing it.
- Random Characters of arg length
- Specific Characters of arg string
- Random Colors
- Color tiers.  All tiles are green but when we run out of tiles use first tile but now blue. etc.
- Random color tiers.
- Reveal an image.
- Tile decay.
- Logarithmic Tile step up.... First step is '.', second is ',', fourth is '*', etc.


# Build
gcc -o whatever_you_want ascii-walker.c -lncurses

# run

./whatever_you_want <sleep_time> <decay, 0 or 1> <number of drunkards>

sleep time is default 40000
if decay is 1 tiles will slowly decay to a lower tile
number of drunkards, obviously 1 is default


