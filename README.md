# Ricoh firmware decoder

Some people tried to brute force but the logic can be found by looking at the file.
If type is container simply run ricohdec again on generated file...

Ricoh Theta firmware:

```
$ ./ricohdec ./gy1_v162.frm

filelen=6452736 dircount=2
flen=3749248 filename=y02firm6.bin mode=encrypted
flen=2701312 filename=gy1_v162up.bin mode=container

$ ./ricohdec ./gy1_v162up.bin
filelen=2701312 dircount=1
flen=2698836 filename=y02firm8.bin mode=encrypted
```

Ricoh GR firmware:

```
$ ./ricohdec ./rg1_v051.frm

filelen=12471808 dircount=2
flen=6643200 filename=b01firm6.bin mode=encrypted
flen=5826560 filename=rg1_v001up.bin mode=container

$ ./ricohdec ./rg1_v001up.bin
filelen=5826560 dircount=1
flen=5824084 filename=b01firm8.bin mode=encrypted
```
