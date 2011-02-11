
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

@
.macro	throwSystemBoot
	.word	0xE6000010
	.endm

@
.macro	throwExitToShell
	.word	0xE6000110
	.endm

@
.macro	throwDebugger
	.word	0xE6000210
	.endm

@
.macro	throwDebugStr
	.word	0xE6000310
	.endm

@
.macro	throwPublicFiller
	.word	0xE6000410
	.endm

@
.macro	throwSystemPanic
	.word	0xE6000510
	.endm

@
.macro	throwSendTestResults
	.word	0xE6000710
	.endm

@
.macro	throwTapFileCntl
	.word	0xE6000810
	.endm


@ NewtonScript bytecode
.macro	BCPop
	.byte	0000
	.endm
.macro	BCDup
	.byte	0001
	.endm
.macro	BCReturn
	.byte	0002
	.endm	
.macro	BCPushSelf
	.byte	0003
	.endm
.macro	BCSetLexScope
	.byte	0004
	.endm
.macro	BCIterNext
	.byte	0005
	.endm
.macro	BCIterDone
	.byte	0006
	.endm
.macro	BCPopHandlers
	.byte	0007
	.endm
.macro	BCGetPath
	.byte	0220
	.endm
.macro	BCGetPathPush
	.byte	0221
	.endm
.macro	BCSetPath
	.byte	0230
	.endm
.macro	BCSetPathPush
	.byte	0231
	.endm
.macro	BCCommand cmd, val
	.if	(\val<7) && (\val>=0)
	.byte	\cmd + \val
	.else
	.byte	\cmd+7, (\val>>8)&255, \val&255
	.endif
	.endm
.macro	BCPush	val
	BCCommand 0030, \val
	.endm
.macro	BCPushConstant	val
	BCCommand 0040, \val
	.endm
.macro	BCCall	val
	BCCommand 0050, \val
	.endm
.macro	BCInvoke val
	BCCommand 0060, \val
	.endm
.macro	BCSend val
	BCCommand 0070, \val
	.endm
.macro	BCSendIfDefined val
	BCCommand 0100, \val
	.endm
.macro	BCResend val
	BCCommand 0110, \val
	.endm
.macro	BCResendIfDefined val
	BCCommand 0120, \val
	.endm
.macro	BCBranch val
	BCCommand 0130, \val
	.endm
.macro	BCBranchL val
	.byte	0137, \val/256, \val&255
	.endm
.macro	BCBranchIfTrue val
	BCCommand 0140, \val
	.endm
.macro	BCBranchIfFalse val
	BCCommand 0150, \val
	.endm
.macro	BCFindVar	val
	BCCommand 0160, \val
	.endm
.macro	BCGetVar	val
	BCCommand 0170, \val
	.endm
.macro	BCMakeFrame	val
	BCCommand 0200, \val
	.endm
.macro	BCMakeArray	val
	BCCommand 0210, \val
	.endm
.macro	BCSetVar	val
	BCCommand 0240, \val
	.endm
.macro	BCFindAndSetVar	val
	BCCommand 0250, \val
	.endm
.macro	BCIncrVar	val
	BCCommand 0260, \val
	.endm
.macro	BCBranchIfLoopNotDone	val
	BCCommand 0270, \val
	.endm
.macro	BCFreqFunc	val
	BCCommand 0300, \val
	.endm
.macro	BCNewHandlers	val
	BCCommand 0310, \val
	.endm

@ frequent functions
.macro	BCAdd
	.byte 0300
	.endm
.macro	BCSubtract
	.byte 0301
	.endm
.macro	BCAref
	.byte 0302
	.endm
.macro	BCSetAref
	.byte 0303
	.endm
.macro	BCEquals
	.byte 0304
	.endm
.macro	BCNot
	.byte 0305
	.endm
.macro	BCNotEqual
	.byte 0306
	.endm
.macro	BCMultiply
	.byte 0307, 0, 7
	.endm
.macro	BCDivide
	.byte 0307, 0, 8
	.endm
.macro	BCDiv
	.byte 0307, 0, 9
	.endm
.macro	BCLessThan
	.byte 0307, 0, 10
	.endm
.macro	BCGreaterThan
	.byte 0307, 0, 11
	.endm
.macro	BCGreaterOrEqual
	.byte 0307, 0, 12
	.endm
.macro	BCLessOrEqual
	.byte 0307, 0, 13
	.endm
.macro	BCBitAnd
	.byte 0307, 0, 14
	.endm
.macro	BCBitOr
	.byte 0307, 0, 15
	.endm
.macro	BCBitNot
	.byte 0307, 0, 16
	.endm
.macro	BCNewIterator
	.byte 0307, 0, 17
	.endm
.macro	BCLength
	.byte 0307, 0, 18
	.endm
.macro	BCClone
	.byte 0307, 0, 19
	.endm
.macro	BCSetClass
	.byte 0307, 0, 20
	.endm
.macro	BCAddArraySlot
	.byte 0307, 0, 21
	.endm
.macro	BCStringer
	.byte 0307, 0, 22
	.endm
.macro	BCHasPath
	.byte 0307, 0, 23
	.endm
.macro	BCClassOf
	.byte 0307, 0, 24
	.endm







