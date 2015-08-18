; Attempt at I/O
tps=177564 ; control register for console output
tpb=177566 ; data register for console output
.origin 1000
; r0 etc all start with zero so do not need to initialize
start: movb msg(r0),r1
   beq finish ; zero byte at end of message
   movb r1,tpb
   incb r0
; Now wait for character to print, this might take along time
wtc:tstb tps
  bpl wtc
  br start
finish: nop
  halt
msg: .string "Hello World"
.end start
