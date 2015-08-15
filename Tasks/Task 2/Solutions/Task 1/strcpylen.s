; Program to copy and determine length of string
.origin 1000
start: mov #msga, r1
   mov #msgb, r2
   clr r0
l1: movb (r1)+, (r2)+
   beq done
   inc r0
   br l1
done: halt
msga: .string "A string whose length is to be determined"
msgb: .string "Different string that should get overwritten"
.end start

