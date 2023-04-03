# Wagic: War of the Homebrew

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
meson setup --cross-file ./tools/meson/psp.build --buildtype release build/psp
meson compile -v -C build/psp
```
