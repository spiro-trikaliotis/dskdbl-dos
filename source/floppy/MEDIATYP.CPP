/* $Id: MEDIATYP.CPP,v 1.3 2005/01/10 10:47:37 trikalio Exp $

  File: MEDIATYP.CPP
        ============

Author: Spiro Trikaliotis (trikalio)
Date:   06.08.1996
*/
/*
Set the media type (for format)
*/

#include "fdisk.hh"

bool TFloppyDisk::SetMedia( int cylinder, int sector ) {
      _DL = driveno;
      _CH = --cylinder;
      _CL = sector;
      _AH = 0x18;
  asm int 0x13
  asm jnb short NoError // No error occured? Then jump!
      driveerror = (TError) _AH;
      return true;

NoError:
      driveerror = NO_ERROR;
      return false;
}
