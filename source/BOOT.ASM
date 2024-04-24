comment!*********************************************************************
*** $id$
***
*** BOOT-Block eines Massenspeichers (Diskette)
*** zum Laden von OS/386
****************************************************************************!

	ideal
	p8086
	%pagesize       65,80

	locals  @@

	WriteOffset = 200h
	base0    = 7c00h
	base1    = base0+WriteOffset

Macro   WriteString adr,len,col
	mov     bp,adr
	mov     bx,col
	mov     cx,len
	Call    WriteStr
EndM

Segment Code
	org     base1

	assume cs:code

Start:  jmp     DoBoot  ; Sprung zur eigentlichen Boot-Routine

	db      'Spiro R.'

BytePerSctr     dw      512     ; bytes per sector
SctrPerClstr    db      1       ; sectors per cluster
ReservedSctr    dw      1       ; no of reserved sectors
NumberFATs      db      2       ; no of FATs
NoDirEntries    dw      0E0h    ; no of directory entries
NoSectors       dw      0b40h   ; no of sectors in volume
MediaDesc       db      0F0h    ; media descriptor
SctrPerFAT      dw      9       ; no of sectors per FAT
SctrPerTrack    dw      18      ; no of sectors per track
NoHeads dw      2       ; no of heads

NoFirstSctr     dd      0       ; no of first sector in volume (low word)
	dw      0       ; location 020h (???)
	dw      0       ; location 022h (???)

NoOfDisk        db      0h      ; disk number for access via INT 13h
		db      01h     ; location 025h (???)
SerNoMark       db      0h      ; marker: serial-no is valid (=29h)
SerNoLow        dw      0000h   ; serial-no (lower)
SerNoHigh       dw      0000h   ; serial-no (higher)

	db      'Trikaliotis'
	db      'FAT     '

TrikSoftText:
	db      13,10,13,10,'This disk was formatted using Disk-Doubler, '
	db      13,10,'a toolkit for handling floppy disks.'
	db      13,10,'[unregistered Shareware Version]'
	db      13,10,13,10,'Written 1996 by Spiro Rolf Trikaliotis. '
	db      'All rights reserved.',13,10
	db      13,10 ; ,'Press any key to boot from hard disk',13,10,10
TrikSoftTextEnd:

ErrorText:
	db      13,10,13,10,'Error trying to boot from hard disk.'
	db      'Halting system!',13,10
ErrorTextEnd:


DoBoot: mov     ax,cs
	mov     ds,ax
	assume  ds:code
	mov     es,ax
	assume  es:code

	; set stack
	mov     ss,ax
	assume  ss:code
	mov     sp,base1+400h 

	; copy the boot block to a secure location
	mov di,base1
	mov si,base0
	mov cx,200h
	cld
	rep movsb

	jmp near $+WriteOffset+03h

	WriteString     <offset TrikSoftText>,<TrikSoftTextEnd-TrikSoftText>,7

	Xor     ax,ax
	mov     dx,80h
	Int     13h     ; AX=0: RESET Disk System
	Jb      Short BootError ; jump if error!

	MOV     BX,base0        ; Adresse, wohin gelesen werden soll
	MOV     DX,80h          ; lese von Festplatte 0x80, Head 0
	MOV     CX,1            ; Zylinder 0, Sector 1
	mov     ax,0201h
	int     13h             ; Lese den Fehlercode
	JB      Short BootError

	; Zeichen auslesen -> l”schen
;       sti
;       Mov Ah,0
;       Int 16h
;       cli

;        mov ax,500
;DoWait: dec cx
;        cmp cx,0
;        jne DoWait
;        Dec Ax
;        Cmp Ax,0
;        Jne DoWait

	xor ax,40h
	mov es,ax

	mov bx,[es:6ch]
	mov cx,[es:6eh]
	add bx,18*5
	adc cx,0

	mov ax,[es:1Ah]

DoWait: cmp [es:6eh],cx
	ja  short QuitWait 
	cmp [es:6ch],bx
	ja  short QuitWait 

	cmp ax,[es:1Ch]
	je Short DoWait

QuitWait:
	mov [es:01Ch],ax

	jmp     near 07c00h

BootError:
	WriteString     <offset ErrorText>,<ErrorTextEnd-ErrorText>,7
stop:   jmp     stop


Proc    WriteStr

	Push    cx
	push    bp
	Mov     ah,03
	int     10h     ; dh/dl enthalten nun zeile/spalte des cursors!
	pop     bp
	Pop     cx

	mov     ax,1301h
	int     10h
	ret

EndP    WriteStr

	org     base1+1feh
	dw      0aa55h

EndS    Code

	End     Start
