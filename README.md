nRF24LE1 programmer
===================

Programs nRF24LE1 SOC via Arduino

Upload sketch to your Arduino board (I use china UNO R3) and connect SPI to nRF24LE1

```Pin-Mapping:
Arduino		24Pin		32Pin		48Pin
D07 (RXD)	12 P0.6		10 P0.4		15 P1.1
D08 (PROG)	 5 PROG		 6 PROG		10 PROG
D09 (RESET)	13 RESET	19 RESET	30 RESET
D10 (FCSN,TXD)	11 P0.5		15 P1.1		22 P2.0
D11 (FMOSI)	 9 P0.3		13 P0.7		19 P1.5
D12 (FMISO)	10 P0.4		14 P1.0		20 P1.6
D13 (FSCK)	 8 P0.2		11 P0.5 	16 P1.2
```


Now run programmer and upload your firmware:
```Programmer.exe \\.\COM13 r main.ihx```

You can also dump nRF24LE1 memory (see dump_example.txt):
```Programmer.exe \\.\COM13 d main.ihx```


This work based on code https://github.com/DeanCording/nRF24LE1_Programmer
Thanks, DeanCording.
