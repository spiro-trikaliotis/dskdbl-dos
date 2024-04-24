/* $Id: INIT.CPP,v 1.3 2005/01/10 10:47:37 trikalio Exp $

  File: INIT.CPP
        ========

Author: Spiro Trikaliotis (trikalio)
Date:   06.08.1996
*/
/*
Initialize floppy disk
*/

#include "fdisk.hh"

bool TFloppyDisk::Init( void ) {
      _DL = driveno;
      _AH = 0x00;
  asm int 0x13
  asm jnb short NoError // No error occured? Then jump!
      driveerror = (TError) _AH;
      return true;

NoError:
      driveerror = NO_ERROR;
      return false;
}
