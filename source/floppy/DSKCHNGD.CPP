/* $Id: DSKCHNGD.CPP,v 1.2 2005/01/10 10:47:37 trikalio Exp $

  File: DSKCHNGD.CPP
        ============

Author: Spiro Trikaliotis (trikalio)
Date:   06.08.1996
*/
/*
Detect a floppy disk change
*/

#include "fdisk.hh"

bool TFloppyDisk::DiskChanged( void ) {
      _DL = driveno;
      _AH = 0x16;
  asm int 0x13
      if (_AH==6)
        return false;
      else
        return true;
}
