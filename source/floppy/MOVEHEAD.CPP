/* $Id: MOVEHEAD.CPP,v 1.3 2005/01/10 10:47:37 trikalio Exp $

  File: MOVEHEAD.CPP
        ============

Author: Spiro Trikaliotis (trikalio)
Date:   06.08.1996
*/
/*
Move the floppy disk's r/w-head
*/

#include "fdisk.hh"

bool TFloppyDisk::MoveHead( int side, int cylinder ) {
      _DL = driveno;
      _DH = side;
      _CX = cylinder;
      _AH = 0x0C;
  asm int 0x13
  asm jnb short NoError // No error occured? Then jump!
      driveerror = (TError) _AH;
      return true;

NoError:
      driveerror = NO_ERROR;
      return false;
}
