/* $Id: READ.CPP,v 1.4 2005/01/10 10:47:37 trikalio Exp $

  File: READ.CPP
        ========

Author: Spiro Trikaliotis (trikalio)
Date:   06.08.1996
*/
/*
Read some sectors from floppy disk
*/

#include "fdisk.hh"

bool TFloppyDisk::ReadSector( PBuffer buffer, int side,
                              int cylinder,   int sector, int count ) {
      _DL = driveno;
      _DH = side;
      _CH = cylinder;
      _ES = ((long)buffer) >> 16;
      _BX = ((int) buffer) & 0xFFFF;
      _CL = sector;
      _AL = count;
      _AH = 0x02;
  asm int 0x13
  asm jnb short NoError // No error occured? Then jump!
      driveerror = (TError) _AH;
      return true;

NoError:
      driveerror = NO_ERROR;
      return false;
}
