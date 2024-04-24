# dskdbl-dos

## What is it?

dskdbl-dos (original name: DSKDBL) is small utility for reading and writing
floppy disks on MS-DOS 3.3 and up.

This is a small utility I wrote in 1996 or so for reading and writing disk
images with a PC, in order to preserve them.

## Why?

These sources are provided mostly for nostalgic reasons. If you want to
preserve images on your own, in almost all cases, you are much better served
with dd on Linux or a BSD variant.

## Usage help

    Disk-Doubler (c) 1996 Spiro Trikaliotis
    Usage: DSKDBL [-?bvrwdfFCI] filename

    Available options:

    -?          Print this help screen
    -b          Print out banner
    -v          Verbose mode

    -r          Read a complete disk into a file
    -w          Write a complete disk from a file
    -c          Compare a disk with a file
    -d:{x}      Work on drive x (default=A:)
    -f:360      Diskformat:  360K
    -f:1200     Diskformat: 1200K
    -f:720      Diskformat:  720K
    -f:1440     Diskformat: 1440K
    -F          Really FORMAT the disk to be written
    -C          Really CLEAR a disk
    -I          Initialize the disk (i.e., rewrite boot/root/FAT)
    -Nxxxxyyyy  Assign the volume serial no. xxxx-yyyy to the disk
                (only with -F, -C, -I)

    <ESC> aborts immediately

:warning: ** When formatting a disk, the boot sector might look unusual **
Back then, I thought it would be nice to have my own boot code. From todays
perspective, this might not be a good idea anymore. This applies to -F, -C
and -I only.

## Usage Examples

Read the contents of a disk into a file called example.img:

    dskdbl -r image.img

Write the contents of an image file example.img to a disk:

    dskdbl -w image.img

Format a disk with the serial number 1234-ABCD:

    dskdbl -F -N1234ABCD

## How to compile?

It was written in C++ with Borland C++ 3.1 (BC++). It should run on any MS-DOS
PC from MS-DOS 3.3 (for support of 3,5" HD) up to MS-DOS 6.22.

Start the Borland C++ compiler with the project file source/DSKDBL.PRJ, and
compile from there.

## License

The original sources from me did not include any license. I put this under
GPL2 for now. As there are no external contributions, I am not sure if this will
remain this way forever.
