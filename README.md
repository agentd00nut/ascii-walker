# ascii-walker

This is a C based drunken walk that wraps around the edges of the terminal screen.
It does not support screen resizing while running but between runs thats fine.

It runs untill you kill it with ctrl-c.

Features
----------

- Six Tiles symbol + color combinatos
- Colors! WoW!
- FAST!  20,000+ steps/s is possible
- Many walkers!

Future Things
--------------

Things i may one day do.

- Ability to feed in an image and have it drunken walk around slowly revealing it.
- Random Characters of arg length
- Specific Characters of arg string
- Random Colors
- Color tiers.  All tiles are green but when we run out of tiles use first tile but now blue. etc.
- Random color tiers.
- Reveal an image.
- Logarithmic Tile step up.... First step is '.', second is ',', fourth is '*', etc.

How To Build
-------------

gcc -o ascii-walker ascii-walker.c -lncurses

How to Run
-----------
./ascii-walker

# Arguments

These are listed in order... unfortunately because I'm lazy if you provide one argument you must provide them all.

- show_players 0,1 | show the "@" signs or not.
- should_decay 0,1 | If the path should decay over time.
- should_cycle 0,1  | If when stepping on the last symbol if it should wrap around to a blank space.
- sleep_time 0-big# | The number of cycles to sleep between steps in the program... defaults to 40000
- players 1-100+ | Number of walkers to have on the map


Editing the visuals
-------------------

Past versions used to use colored characters on a black background.
Current version abandoned that for doing colored backgrounds with no characters... this improves visuals when zoomed out
but some people may prefer the characters... this is very easy to change.

1. At lines 51 - 56 you can set the symbols you'd like the steps to use... just replace ' ' with '.' for example.
2. Uncomment lines 58 through 64.
3. Comment lines 66 through 72.

Now just rebuild and run!

One day i will bring over the changes from viet's branch and include his code to let you define these things from the command line!