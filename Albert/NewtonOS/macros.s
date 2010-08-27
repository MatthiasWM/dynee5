
@ 
@ Create a NewtonScript signed integer value:
@	NSInt	-7
@
.macro  NSInt   val
        .word   (\val<<2)&0xfffffffc
        .endm

@
@ Create a NewtonScript Unicode character
@	NSChar	0x2345
@
.macro  NSChar  val
        .word   (\val<<4)|0x0000000a
        .endm

@
@ Create a NewtonScript pointer to an address in ROM
@	NSPtr	Label27
@
.macro  NSPtr   addr
        .word   \addr+1
        .endm

@
@ Create a NewtonScript NIL of FALSE
@	NSNil
@
.macro  NSNil
        .word   0x00000002
        .endm

@
@ Create a NewtonScript TRUE
@	NSTrue
@
.macro  NSTrue
        .word   0x0000001a
        .endm

@
@ Create a NewtonScript ASCII symbol
@	NSSymbol	0x873453, "Hallo!"
@
.macro  NSSymbol        hash, value
        .word   0x00055552
        .word   \hash
        .asciz  "\value"
        .balign 4, 0xba

        .endm

@
@ Create a NewtonScript Magic Pointer
@	NSMagic	0, 217
@
.macro  NSMagic table, index
        .word   ((\table&0x3ffff)<<14)|((\index&0xfff)<<2)|0x00000003
        .endm

@
@ Create the header for a NewtonScript binary object
@ 'Size' is the number of bytes in the payload
@	NSObjBin	23
@
.macro  NSObjBin        size
        .word   0x00000040|((\size+12)<<8)
        .word   0
        .endm

@
@ Create the header for a NewtonScript binary object (X flag set)
@ 'Size' is the number of bytes in the payload
@	NSObjXBin	23
@
.macro  NSObjXBin        size
        .word   0x000000C0|((\size+12)<<8)
        .word   0
        .endm

@
@ Create the header for a NewtonScript Array object
@ 'Size' is the number of entries in the payload
@	NSObjArray	4
@
.macro  NSObjArray        entries
        .word   0x00000041|((\entries+3)<<10)
        .word   0
        .endm

@
@ Create the header for a NewtonScript Array object (X flag set)
@ 'Size' is the number of entries in the payload
@	NSObjXArray	4
@
.macro  NSObjXArray        entries
        .word   0x000000C1|((\entries+3)<<10)
        .word   0
        .endm

@
@ Create the header for a NewtonScript Frame object
@ 'Size' is the number of entries in the payload
@	NSObjArray	4
@
.macro  NSObjFrame        entries
        .word   0x00000043|((\entries+3)<<10)
        .word   0
        .endm

@
@ Create the header for a NewtonScript Frame object (X flag set)
@ 'Size' is the number of entries in the payload
@	NSObjXArray	4
@
.macro  NSObjXFrame        entries
        .word   0x000000C3|((\entries+3)<<10)
        .word   0
        .endm

@
@ Create a binary object representing a "double" or "real"
@
.macro	NSObjReal	val
	NSObjBin	8
	NSPtr		SYMreal
	.double		\val
	.endm



