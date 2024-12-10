# Don't Kill Santa

This is a game written in C using [raylib](https://github.com/raysan5/raylib), for the for the [12 Seconds of Christmas](https://itch.io/jam/12-seconds-of-christmas-game-jam-2024) game jam.

![Don't Kill Santa](screenshots/dont-kill-santa.png)

## Playing the game

[https://duffn.itch.io/dont-kill-santa](https://duffn.itch.io/dont-kill-santa)

## Building

- Build locally

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release .. # or cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
./dontkillsanta
```

- Build for the web

```
cd tools
./build_for_web.sh
```

## Attributions

- Santa sprite - [pzUH](https://pzuh.itch.io/santa-claus-free-sprites)
- Candy cane sprite - [Fortress Side](https://fortressside.itch.io/christmas-theme-500-sprites)
- Background image and ground tiles - [OpenGameArt - pzUH](https://opengameart.org/content/winter-platformer-game-tileset)
- Raylib CMake boilerplate - [BrettWilsonBDW](https://github.com/BrettWilsonBDW/raylib-cmake)
- Christmas background music - [OpenGameArt - duanehoyt](https://opengameart.org/content/christmas-neo)
- Christmas font - [1001 Free Fonts - KongFont](https://www.1001freefonts.com/hello-santa.font)
- Menu font - [1001 Free Fonts - Typodermic Fonts](https://www.1001freefonts.com/coolvetica.font)

## License

[GNU GPL v3.0](https://www.gnu.org/licenses/gpl-3.0.en.html#license-text)
