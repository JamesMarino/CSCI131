; demo of simplified non-interrupt OS
; 
; It just provides Readline, Writeline, itoa, atoi
; 
; Little 'hello world' application uses its services
;
; data for the teleprinter
tps=177564 ; control register for console output
tpb=177566 ; data register for console output
;
; data for the keyboard
tks=177560 ; control register for keyboard
tkb=177562 ; data register for keyboard
;
.origin 1000
; low level non-interrupt I/O
; getchar
; wait for flag to set
; read the character
getchar:inc @#tks ; enable
getloop: bit #200,@#tks ; wait for done flag
    beq getloop
    movb @#tkb,r0
    return
; putchar - need to echo the character
putchar:mov r0,@#tpb
wtc:	tstb @#tps
  bpl wtc
  return
;
; readline - called with r1 pointing to a buffer in user area where line to be stored
; uses r0
; read characters (echoing them to teleprinter) and store until newline dealt with
; (and add a nul  byte for safety before returning)
readline: call getchar
  call putchar
  movb r0,(r1)+
  cmpb r0,#15
  bne readline
  clr r0
  movb r0,(r1)+
 return
;
; writeline called with r1 pointing to user buffer with message
; uses r0
; sends characters until get a nul
writeline:movb (r1)+,r0
   bne more
   return
more:call putchar
   br writeline
;
; atoi called with r1 pointing to buffer with characters
; processes all digits converting to integer
; will use r3 while doing multiplications
; assumes only short integers so takes only low order part of product
atoi: clr r3
atoil:cmpb (r1),#60
   blt atoiend
   cmpb (r1),#71
   bgt atoiend
; character is a decimal digit
   movb (r1)+,r0
  sub #60,r0
; r0 now holds numeric value 0-9 for next decimal digit
  mul #12,r3
 add r0,r3
 br atoil
; return result in r0
atoiend: mov r3, r0
  return
;
; itoa - this is a non-recursive version
; on entry r0 is number, r1 is address of a buffer that must be 5 words long - last word 0
; will loop filling in characters starting at low order decimal digit
; (+ve numbers only)
; use r2,r3
itoa:mov #10,r2
; start by filling buffer with spaces
itoafill:movb #40,(r1)+
   sob r2,itoafill
; now generate digits
  tst r0
 bgt nonzero
; simply put 0
 movb #60,-(r1)
 return
 nonzero: clr r2
  mov r0,r3
itoadiv: tst r3
  beq itoadone
; do a division
  div #12,r2
; remainder in r3 is next value of next digit to go in buffer
  add #60,r3
  movb r3,-(r1)
; quotient in r2 
  mov r2,r3
  clr r2
  br itoadiv
; itoadone -
itoadone:   return
.origin 2000
application:mov #msg1,r1
  call writeline
  mov #newline,r1
  call writeline
  mov #msg2,r1
  call writeline
  mov #newline,r1
  call writeline
  mov #msg3,r1
  call writeline
  mov #inbuf,r1
 call readline
  mov #inbuf,r1
  call atoi
  mov r0, num1
  mov #msg4,r1
  call writeline
  mov #inbuf,r1
  call readline
  mov #inbuf,r1
  call atoi
  mov r0,num2
  add num1,r0
  mov r0,sum
  mov #msg5,r1
 call writeline
 mov sum,r0
 mov #numbuf,r1
 call itoa
 mov #numbuf,r1
 call writeline
 mov #newline,r1
 call writeline
  halt
;
.origin 2400
; data
msg1: .string "Hello world"
msg2: .string "I am a computer, I do arithmetic"
newline: .word 15
msg3: .string "Enter num1 : "
msg4: .string "Enter num2 : "
msg5: .string "num1 + num2 = "
inbuf: .blkw 20
num1: .blkw 1
num2: .blkw 1
sum: .blkw 1
numbuf: .blkw 5
.end application
