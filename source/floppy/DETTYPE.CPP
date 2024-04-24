/* $Id: DETTYPE.CPP,v 1.3 2005/01/10 10:47:37 trikalio Exp $

  File: DETTYPE.CPP
        ===========

Author: Spiro Trikaliotis (trikalio)
Date:   06.08.1996
*/
/*
Detect the type of the floppy drive
*/

#include "fdisk.hh"

TFloppyDisk::TDriveType TFloppyDisk::DetectType( void ) {
      _DL = driveno;
      _AH = 0x00;
  asm int 0x13
  asm jnb short NoError // No error occured? Then jump!
      driveerror = (TError) _AH;
      return NONE;

NoError:
      driveerror = NO_ERROR;
      return (TDriveType) _AL;
}
