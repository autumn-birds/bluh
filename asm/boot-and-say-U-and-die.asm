;
; Assembly language code for a (very, very) basic program which can be written onto
; a floppy boot sector and run on boot.  It just invokes the BIOS to print
; 'U' to the screen, then stops the computer.
;
; If you wanted to test it you'd need qemu, nasm and a c compiler of some
; kind.  You would then pass the path to this file (relative to your
; cwd) into /utils/flop from a shell prompt.
;
; This was mostly made by adapting an example:
; wiki.osdev.org/Real_mode_assembly_bare_bones

; `org` will not actually pad the file with zeroes (which is good)
; it just declares some starting offset to which all the labels
; will be added/computed relative to. Where the code will start
; when someone DOES load it.
org 0x7c00
bits 16

; Copied verbatim
; This I think sets up segment registers so that memory addresses will make sense.
; I need to figure out the exact semantics of these but for a first test, magic is
; okay enough here.  (You can see I never actually access memory anyway [yet?])
;
; I think these are the registers that you can't predict how (or if???) the BIOS
; will set them up for you.
mov ax, 0
mov ds, ax    ; eg, set both of these to 0:
mov es, ax

; This sets up the stack.  The stack grows 'downwards' (eg, towards address 0)
; ss must be the stack position. sp is the start of the program
; (Note that this means that if we add too much to the stack we'll start to
; clobber the BIOS area, but there are probably no enforced checks for that
; anywhere...)
mov ss, ax ; 0
mov sp, 0x7C00 ; see `org`. This is where BIOS loads the boot sector.

; At this point, all the registers are set up and we can do random
; stuff to see if it worked.
mov ah, 0x0E
mov al, 0x55  ; I THINK a character code goes here.  (It turns out this is ASCII 'U')
int 0x10

; This halts the CPU.  That is, nothing will happen until a next interrupt.
; I think this allows the CPU firmware to decide to turn things off for a
; bit (in CPU time anyway.)
stoppingPoint:
hlt
; I added this here in case somehow we resume here after an interrupt.
jmp stoppingPoint

; Done!
