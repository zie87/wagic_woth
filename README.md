# Wagic: War of the Homebrew
[![build](https://github.com/zie87/wagic_woth/actions/workflows/build.yml/badge.svg)](https://github.com/zie87/wagic_woth/actions/workflows/build.yml)
[![Coverage Status](https://coveralls.io/repos/github/zie87/wagic_woth/badge.svg?branch=main)](https://coveralls.io/github/zie87/wagic_woth?branch=main)
[![License](https://img.shields.io/badge/License-BSD_3--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)

# Build

## Linux build

```
sudo apt install meson libsdl2-dev libpng-dev libjpeg-dev libfreetype-dev libz-dev libboost-all-dev 
```

```
meson setup --buildtype release build/sdl
meson compile -v -C build/sdl
```

## PSP build

```
meson setup --cross-file ./tools/psp/mips_psp.build --buildtype release build/psp
meson compile -v -C build/psp
```

## Windows build

### MinGW 32 Bit (Linux)
```
sudo dnf install -y meson ninja-build wine mingw32-gcc-c++ mingw32-boost
```

```
meson setup --cross-file ./tools/mingw/i686-w64-mingw32.build --buildtype release build/mingw32
meson compile -v -C build/mingw32
```

### MinGW 64 Bit (Linux)
```
sudo dnf install -y meson ninja-build wine mingw64-gcc-c++ mingw64-boost
```

```
meson setup --cross-file ./tools/mingw/x86_64-w64-mingw32.build --buildtype release build/mingw64
meson compile -v -C build/mingw64
```
