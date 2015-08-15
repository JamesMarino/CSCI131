; demo of simplified interrupt OS
; 
;  system calls
;     exit = os executes halt instruction!
;     read = os will read a line from keyboard
;        returning when a newline character read
;     write = os will write a line to teletype
;        returning when line all written (nul character at end)
;    atoi, and itoa - integer<->string conversions
;
; define the operating system calls as trap instructions
exit=104400
readline=104401
writeline=104402
atoi=104403
itoa=104404
; 
; data for the trap instruction
trapaddr=34 ; "interrupt entry point" - start address of request handler
trapsw=36 ; location for status word
opsyssw=40 ; value of status word - priority 1
;
; data for the teleprinter
ttyaddr=64 ; interrupt entry point for tty - set to address of handler routine
ttysw=66 ; holds status word to load when start handling tty event
tpsw=200 ; value to put in status word - priority 4
tps=177564 ; control register for console output
tpb=177566 ; data register for console output
;
; data for the keyboard
kbdaddr=60 ; interrupt entry point for kbd - set to address of handler routine
kbdsw=62 ; holds status word to load when start handling kbd event
kbsw=200 ; value to put in status word 
kbdc=177560 ; control register for console input
kbdb=177562 ; data register for console input
;
.origin 1000
osstart: mov #os,@#trapaddr
  mov #opsyssw, @#trapsw
  mov #tput,@#ttyaddr
  mov #tpsw,@#ttysw
  mov #kget,@#kbdaddr
  mov #kbsw,@#kbdsw
;
; need to enable interrupts from keyboard and teletype
; set 'enable' and 'done' in tty
; enable only in kbd
 mov #300,@#tps
 mov #100,@#kbdc
;
; hopefully all is ready
; start the application
  jmp application
; -----------------
; handle keyboard interrupt
kget:movb @#kbdb,ch
   movb ch, @ibufptr
  inc ibufptr
   cmpb #15,ch
   beq ilinedone
   rti
; ilinedone - add the nul byte, set flag saying input ready
ilinedone: clrb @ibufptr
  inc kbdoneflag
  rti
;
; os variables
ibufptr:.blkw 1
kbdoneflag:.blkw 1
ch:.blkw 1
; ------------
; handle teleprinter interrupt
tput:tstb @obufptr
  beq msgdone
; There is another character to go
  movb @obufptr,@#tpb
  inc obufptr
  rti
msgdone:inc printdoneflag
  rti
;
; os variables
obufptr: .blkw 1
printdoneflag: .blkw 1
; --------------------------------------------
; my micro operating system
; I will be using r0 and r1 (and maybe other registers) so save these
os:mov r0,-(sp)
  mov r1,-(sp)
; find out which request - pick up return address as saved in stack
  mov 4(sp),r1
; program counter has been incremented - take off 2
  dec r1
  dec r1
; r1 should hold the address of the trap instruction
  mov (r1),r0
; r0 now holds the actual trap instruction that was executed
; bottom 8 bits contain request id - (though typically far fewer 
; than 255 calls defined)
; clear the top byte
  bic #177400,r0
; convert index to byte offset
  clc ; just in case its set!  
  rol r0
  jmp @osfns(r0)
;
; handle return from os call
; when reach here r0 should hold number of arguments used
; by last os call; need to adjust return address that is on stack
osreturn:clc
  rol r0
  add r0,4(sp)
; and put back registers
  mov (sp)+,r1
  mov (sp)+,r0
  rti
;
; function table for my os
osfns: exitos
  read
  write
  getint
  putint
;
; exit from os
exitos: nop
  nop
  halt
  br exitos ; no escape
; read 
;    takes one argument - address of buffer where input to be stored
;    (argument will be in location immediately after trap instruction)
; uses r0 and r1
; read characters (echoing them to teleprinter) and store until newline dealt with
; (and add a nul  byte for safety before returning)
; on entry r1 still holding address with trap instruction
read: inc r1
  inc r1
; r1 now holds address that stores address of buffer
; make it store the address of buffer
  mov (r1),ibufptr
  clr kbdoneflag
  inc @#kbdc 
; now get wait in OS - interrupt handled keyboard
; will eventually set the 'line done flag'
kbwait:tst kbdoneflag
 bgt kblinedone
 wait
; returns from wait state after interrupt handled
; go back and re-check if line complete
 br kbwait
; finally - the line has been read; can return to user
kblinedone:mov #1,r0
  br osreturn
;
; write
; set up for interrupt driven output
; (initialize buffer pointer, set done flag to false etc)
write:inc r1
  inc r1
  mov (r1),obufptr
  clr printdoneflag
; send the first character
  movb @obufptr,@#tpb
  inc obufptr
; now wait in os until printdoneflag is set
wrtwait:tst printdoneflag
  bgt olinedone
; nothing to do - it's kind of wait loop
  wait
  br wrtwait
olinedone:mov #1,r0
  jmp osreturn
;
; getint 
; processes all digits converting to integer
; will use r3 while doing multiplications (so save and restore)
; assumes only short integers so takes only low order part of product
; has a local variable (valptr)
getint: mov r3,-(sp)
 inc r1
 inc r1
 mov (r1),valptr
 inc r1
 inc r1
 mov (r1),r1
 clr r3
getintl:cmpb (r1),#60
   blt getintend
   cmpb (r1),#71
   bgt getintend
; character is a decimal digit
   movb (r1)+,r0
  sub #60,r0
; r0 now holds numeric value 0-9 for next decimal digit
  mul #12,r3
 add r0,r3
 br getintl
; result in r3; put it where it should go
getintend: mov r3,@valptr
; replace r3 with saved value
  mov (sp)+,r3
; note use of 2 args
  mov #2,r0
  br osreturn
;
valptr: .word 0
; putint - this is a non-recursive version
; use r2,r3 and same local variable valptr
putint:mov r2,-(sp)
  mov r3,-(sp)
  inc r1
  inc r1
  mov (r1),valptr
  inc r1
  inc r1
  mov (r1),r1
  mov @valptr,r0
  mov #10,r2
; start by filling buffer with spaces
putintfill:movb #40,(r1)+
   sob r2,putintfill
; now generate digits
  tst r0
 bgt nonzero
; simply put 0
 movb #60,-(r1)
 br putintdone
 nonzero: clr r2
  mov r0,r3
putintdiv: tst r3
  beq putintdone
; do a division
  div #12,r2
; remainder in r3 is next value of next digit to go in buffer
  add #60,r3
  movb r3,-(r1)
; quotient in r2 
  mov r2,r3
  clr r2
  br putintdiv
; putintdone -
; replace r3 and r2
putintdone:  mov (sp)+,r3
 mov (sp)+,r2
; 2 args
 mov #2,r0 
br osreturn
.origin 2000
; Place Application in here
application:
	readline
	input
	call createreverse
	writeline
	output
	exit
createreverse:
	mov #input, r1
	strlen: tstb (r1)+
	bne strlen
	dec r1
	mov #output, r2
	add r1, r2
	sub #input, r2
	mov #input, r1
cpy:
	movb (r1)+, -(r2)
	bne cpy
	return
.origin 3000
; Data
input: .blkw 100
output: .blkw 100
numbuf: .blkw 5
.end osstart
