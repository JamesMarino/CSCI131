; ----------------------------- IO Library -----------------------------
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
; ------------------ Place Application in here ------------------
;
; Function: Calculate the mean
meanc:
	;
	; Clear registers before use
	clr r0
	clr r1
	clr r2
	clr r3
	clr r4
	clr r5
	; Load size of array in r3
	mov #datasize, r4
	; Load the address of the array into r0
	mov #dataset, r0
	;
	; Start Loop
	loop:
		; Move the current value of r0 into r2
		mov @r0, r2
		;
		; Work with the current value
		; Self add to r3
		add r2, r3
		;
		; Add 2 bytes to the array pointer
		add #2, r0
		; subtract and branch if counter is equal to 0
		; i.e. go back to the loop statement if not done
		sob r4, loop
	;
	; Divide by total addition of numbers
	clr r0
	; arg1: the value to be divided
	; arg2: where the remainder will end up
	mov r3, r1
	; arg1: Divide what you are going to divide by
	; arg2: where the quotiant will be stored
	div #datasize, r0
	;
	; Move the register holding final mean value
	; Into mean variable
	mov r0, meanv
	return
;
; Function: Print the mean
mean:
	; Process data
	call meanc
	; Print out values
	writeline
	msgmean
	itoa
	meanv
	numbuf
	writeline
	numbuf
	return
;
; Function: Calculate the mode
modec:
	; Set highest value in memory
	highestvalue=0
	;
	; Clear registers before use
	clr r0
	clr r1
	clr r2
	clr r3
	clr r4
	clr r5
	;
	; ----- Register Layout -----
	; r0 and r1: Load the address of the array
	mov #dataset, r0
	mov #dataset, r1
	;
	; r2 and r3: Load size of array (as counters)
	mov #datasize, r2
	mov #datasize, r3
	;
	; r4: Current count of equal values in array
	; r5: Highest count of values
	; highestvalue: Highest Value so far --> stored in ram
	;
	; Loop 1 (All numbers in array)
	loop1:
		;
		; Clear the previous loops equal counter r4
		clr r4
		;
		; Loop 2: Again (All numbers in array)
		loop2:
			;
			; ---------- Loop 2 Contents ------------------
			;
			; Branch if current loop 1 array value is equal
			; to current list in loop 2 is traversing
			cmp @r0, @r1
				; Branch of equal
				beq equalinc
				bne continueequalinc
			; Function if equal
			equalinc:
				; Add to the current count
				inc r4
			; Not Equal
			continueequalinc:
			;
			; ---------- Loop 2 Contents ------------------
			;
			; Add 2 bytes to the array pointer
			add #2, r1
			; subtract and branch if counter is equal to 0
			; i.e. go back to the loop statement if not done
			sob r3, loop2
		;
		; ---------- Loop 1 Contents ------------------
		;
		; Compare r4 (current count) with highest value
		cmp r4, r5
			bgt storehigh
			blt continuestorehigh
		;
		; Function if greater than
		storehigh:
			; Move value of r4 into r5
			mov r4, r5
			; Update actual value
			mov @r0, highestvalue
		; Continue function
		continuestorehigh:
		;
		;
		; ---------- Loop 1 Contents ------------------
		;
		;
		; Reset r3
		mov #datasize, r3
		; Reset r1 array to begining
		mov #dataset, r1
		;
		; Add 2 bytes to the array pointer
		add #2, r0
		; subtract and branch if counter is equal to 0
		; i.e. go back to the loop statement if not done
		sob r2, loop1
	;
	; Move highestvalue into buffer
	mov highestvalue, modev
	;
	return
;
; Function: Print the mode
mode:
	; Process data
	call modec
	; Print out values
	writeline
	msgmode
	itoa
	modev
	numbuf
	writeline
	numbuf
	return
;
; Function: Calculate the max and min
maxandminc:
	;
	; Clear registers before use
	clr r0
	clr r1
	clr r2
	clr r3
	clr r4
	clr r5
	;
	; ----- Register Layout -----
	; r0: Load the address of the array
	mov #dataset, r0
	;
	; r1: Load size of array (as counters)
	mov #datasize, r1
	;
	; r2: Current array value
	; r3: Current smallest value
	; r4: Current largest value
	;
	; Store pre loop max and min values (opposites)
	; for initial compare
	mov #77777, r3
	mov #00000, r4
	;
	; Loop 1 (All numbers in array)
	loopmaxmin:
		;
		; ---------- Loop Contents ------------------
		;
		; Get current array value
		mov @r0, r2
		;
		; Compare the min value
		cmp r2, r3
			; Branch if less than
			blt minstore
			;
		; Compare max value
		cmp r2, r4
			; Branch greater than
			bgt maxstore
			;
		; Compare equal to
		cmp r2, r3
			beq continue
		cmp r2, r4
			beq continue
			;
		; Compare in between
		cmp r2, r3
			bgt continue
		cmp r2, r4
			blt continue
			;
		; Store min value
		minstore:
			mov r2, r3
			jmp continue
		;
		; Store max value
		maxstore:
			mov r2, r4
		;
		; Continue
		continue:
		;
		; ---------- Loop Contents ------------------
		;
		; Add 2 bytes to the array pointer
		add #2, r0
		; subtract and branch if counter is equal to 0
		; i.e. go back to the loop statement if not done
		sob r1, loopmaxmin
	;
	; Store values into buffer
	mov r3, minv
	mov r4, maxv
	;
	return
;
; Function: Print the max and min
maxandmin:
	; Process data
	call maxandminc
	; Print out values
	; Max Values
	writeline
	msgmax
	itoa
	maxv
	numbuf
	writeline
	numbuf
	;
	; Newline
	writeline
	newline
	;
	; Min Values
	writeline
	msgmin
	itoa
	minv
	numbuf
	writeline
	numbuf
	; Return
	return
;
; Function: int main()
application:
	; Main Code
	call mean
	;
	writeline
	newline
	;
	call mode
	;
	writeline
	newline
	;
	call maxandmin
	;
	exit
.origin 3000
;
; Strings
msgmean: .string "Mean  : "
msgmode: .string "Mode  : "
msgmax: .string "Max  : "
msgmin: .string "Min  : "
;
; Variables
meanv: .blkw 1
modev: .blkw 1
maxv: .blkw 1
minv: .blkw 1
;
; Data
dataset: .word 644, 4, 105, 6, 6, 644, 6, 644, 644, 2, 3, 105, 11, 1, 1
datasize=17
;
; IO Data
newline: .word 15
input: .blkw 100
output: .blkw 100
numbuf: .blkw 5
.end osstart
