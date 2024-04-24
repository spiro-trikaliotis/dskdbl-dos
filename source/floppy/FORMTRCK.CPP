/* $Id: FORMTRCK.CPP,v 1.4 2005/01/10 10:47:37 trikalio Exp $

  File: FORMTRCK.CPP
        ============

Author: Spiro Trikaliotis (trikalio)
Date:   06.08.1996
*/
/*
Format one track on floppy disk
*/

#include "fdisk.hh"

bool TFloppyDisk::Format1Track ( PFormBuffer buffer,
                                 int side, int cylinder, int count ) {
      _DL = driveno;
      _DH = side;
      _CH = cylinder;
      _ES = ((long)buffer) >> 16;
      _BX = ((int) buffer) & 0xFFFF;
      _AL = count;
      _AH = 0x05;
  asm int 0x13
  asm jnb short NoError // No error occured? Then jump!
      driveerror = (TError) _AH;
      return true;

NoError:
      driveerror = NO_ERROR;
      return false;
}
