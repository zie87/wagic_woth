First build with Meson for PSP:

```
meson setup --cross-file ./psp_cross.txt --buildtype release build/psp
meson compile -v -C ./build/psp/
```

```
[0]        408 bytes | PARAM.SFO
[1]      15601 bytes | /home/user/workspace/projects/mtg/../../projects/mtg/icon.png
[2]          0 bytes | NULL
[3]       5353 bytes | /home/user/workspace/projects/mtg/../../projects/mtg/pic0.png
[4]     208306 bytes | /home/user/workspace/projects/mtg/../../projects/mtg/pic1.png
[5]          0 bytes | NULL
[6]    4762246 bytes | projects/mtg/wagic.prx
[7]          0 bytes | NULL
```
