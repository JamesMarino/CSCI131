; Explination is commmented as follows
; Start memory block write from octal location 01000
.origin 1000
; Add sybmols with octal values to symbol table
maxval=77777
minval=100000
len=20
; Label start
start:
  ; Move address of the label into following labels
  mov #minval,max
  mov #maxval,min
  mov #len,r2
  mov #data,r0
; Loop and compare the address of r0 with max label
loop: cmp @r0,max
  ; Branch if larger than, goto notlarge label
  blt notlarge
  mov @r0,max
notlarge: cmp @r0,min
  bgt notsmall
  mov @r0,min
; If is not small, add value 2 to r0
notsmall: add #2,r0
  ; subtact one and branch if not 0, taking away one from r2 (len) and loop if not 0
  sob r2,loop
  halt

; Data
max: .word 0
min: .word 0
data: .word 167776, 317, 4051, 67676, 174210, 74, 7776, 7, 147333, 31410, 172315, 5612, 31013, 23712, 555, 177204
.end start
