# This is a test program for the assembler and emulator. Every instruction is included here at least once.

lia 0x280
li r0,3
sw r0,0
addi r0,4
sw r0,2
xori r0,10
sw r0,4
ori r0,35
sw r0,6
andi r0,60
sw r0,8

li r0,0
lw r1,0
addia 2
add r0,r0,r1
xori r1,0x2c
bq r1,-15

lia 0x280
m r1,r0
sw r1,10

eb r3,r1
et r4,r1
sw r3,12
sw r4,14

li r2,3
shl r1,r1,r2
sw r1,16
shli r1,r1,5
sw r1,18
shr r1,r1,r2
sw r1,20
shri r1,r1,2
sw r1,22

eb r3,r1
lw r1,20
et r4,r1
sw r3,24
sw r4,26

m r0,r1
lw r1,8

xor r2,r0,r1
sw r2,28
sub r2,r1,r0
mfov r3
sw r2,30
sw r3,32
add r2,r0,r1
mfov r3
sw r2,34
sw r2,36
or r2,r0,r1
sw r2,38
and r2,r0,r1
sw r2,40

li r3,0x80
adda r3
sw r3,0

lias 0
mfar r4,2
xori r4,0x380
mtar r4,1
mtar r7,16

li r4,0
sa r4,0x400
la r4,0x300
sa r4,0x403
la r4,0x400

add r2,r4,r3
lia 0x2b0
mfir r1
sw r1,0

li r1,1
sb r1,2
addi r1,1
lb r2,2
shl r1,r2,r1
sb r1,3
lw r1,2

addi r1,0x3000
bp r1,-6

m r2,r1
addi r1,-0x400
bn r1,-6

sw r1,4
li r1,0x70

shri r1,r1,1
bq r1,-6

li r4,0x80
addi r1,35
and r3,r1,r4
bz r3,-9

sw r2,6
sw r1,8

addi r7,1
m r0,r7
andi r0,2
bq r0,6
lias 1
r

l 3
b 18
li r3,0
wr r3,r3
rr r3,r3
wi r3,1000
ri r3,1000
b 3
r

li r0,0x102c
sl r0,0
ll r0,3
xor r0,r0,r0	# same as li r0,0

lias 0
lia 0
ja 0x15c
b 3
jr r0,0x159

nop
int 7