# dskdbl-dos
A small utility for reading and writing floppy disks on MS-DOS 3.3 and up

This is a small utility I wrote in 1996 or so for reading and writing disk
images with a PC, in order to preserve them.

It was written in C++ with Borland C++ 3.1 (BC++). It should run on any MS-DOS
PC from MS-DOS 3.3 (for support of 3,5" HD) up to MS-DOS 6.22.

This source are provided mostly for nostalgic reasons. If you want to preserve
images on your own, in almost all cases, you are much better served with dd on
Linux or a BSD variant.

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
