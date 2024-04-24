/* $Id: FIXDISK.CPP,v 1.3 2005/01/10 10:47:37 trikalio Exp $

  File: FIXDISK.CPP
        ===========

Author: Spiro Trikaliotis (trikalio)
Date:   06.08.1996
*/
/*
Fix the type of the floppy disk
(only needed for formatting a new disk)
*/

#include "fdisk.hh"

bool TFloppyDisk::FixDiskType( TFloppyType type ) {
      _AL = type;
      _AH = 0x17;
  asm int 0x13
  asm jnb short NoError // No error occured? Then jump!
      driveerror = (TError) _AH;
      return true;

NoError:
      driveerror = NO_ERROR;
      return false;
}
