Playing with unity build. Tried if a unity build is possible needs to fix some macros (min, max) but than it worked:

`meson setup --reconfigure --cross-file ./tools/psp/mips_psp.build --buildtype release --unity off build/psp/release`
```
[0]        408 bytes | woth/PARAM.SFO
[1]      15601 bytes | /home/user/workspace/tools/psp/data/icon.png
[2]          0 bytes | NULL
[3]       5353 bytes | /home/user/workspace/tools/psp/data/pic0.png
[4]     208306 bytes | /home/user/workspace/tools/psp/data/pic1.png
[5]          0 bytes | NULL
[6]    4824278 bytes | woth/wagic.prx
[7]          0 bytes | NULL
```

`meson setup --reconfigure --cross-file ./tools/psp/mips_psp.build --buildtype release --unity on build/psp/unity`
```
[0]        408 bytes | woth/PARAM.SFO
[1]      15601 bytes | /home/user/workspace/tools/psp/data/icon.png
[2]          0 bytes | NULL
[3]       5353 bytes | /home/user/workspace/tools/psp/data/pic0.png
[4]     208306 bytes | /home/user/workspace/tools/psp/data/pic1.png
[5]          0 bytes | NULL
[6]    5474942 bytes | woth/wagic.prx
[7]          0 bytes | NULL
```
