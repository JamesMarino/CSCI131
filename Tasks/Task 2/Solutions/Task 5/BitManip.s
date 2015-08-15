; Author: jm617
.origin 01000
start:
  ; AND
  ; c = a & b
  mov a, c
  ; Complement: dest = ~dest
  com r0
  ; Bit clear: dest &= ~src
  bic r0,c
  ; OR
  ; d = a | b
  mov a, d
  ; BIt Set Logical OR
  bis b, d
  ; XOR
  mov a, e
  mov b, r0
  xor r0, e
halt
a: .word 53425
b: .word 25253
c: .word 0
d: .word 0
e: .word 0
.end start
