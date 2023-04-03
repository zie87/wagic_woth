# Wagic: War of the Homebrew

# Build

## PSP build

```
meson setup --cross-file ./tools/meson/psp.build --buildtype release build/psp
meson compile -v -C build/psp
```
