/* $Id: FDISK.HH,v 1.3 2005/01/10 10:47:37 trikalio Exp $

  File: FDISK.HH
        ========

Author: Spiro Trikaliotis (trikalio)
Date:   06.08.1996
*/
/*
Header for accessing the floppy disk
*/

#ifndef __SRT_FDISK_HH__
#define __SRT_FDISK_HH__

#ifndef __LARGE__
#error FDISK.HH needs the large memory model
#endif

#ifndef __bool__
  enum bool { false = (1==0), true = (1==1) };
#endif // #ifndef __bool__

#ifndef __byte__
  typedef unsigned char byte;
#endif // #ifndef __byte__

class TFloppyDisk {
public:
  enum TDrive { FD0 = 0, FD1 = 1 };
  enum TError { NO_ERROR             = 0x00,
                UNKNOWN_COMMAND      = 0x01,
                NO_ADRESS_MARK       = 0x02,
                WRITE_PROTECTED      = 0x03,
                SECTOR_NOT_FOUND     = 0x04,
                RESET_UNSUCCESSFULL  = 0x05,
                INIT_FAILURE         = 0x07,
                DMA_OVERFLOW         = 0x08,
                DMA_SEGMENT_OVERFLOW = 0x09,
                READ_ERROR           = 0x10,
                CRC_ERROR            = 0x11, // Data is read anyway
                CONTROLLER_FAILURE   = 0x20,
                TRACK_NOT_FOUND      = 0x40,
                DRIVE_DOESNT_RESPOND = 0x80, // no disk in drive?
                BIOS_FAILURE         = 0xBB,
                UNKNOWN_ERROR        = 0XFF
  };

private:
  TDrive driveno;
  TError driveerror;

public:
  typedef char PBuffer[];

  enum TBytesPerSector { B128 = 0, B256 = 1, B512 = 2, B1024 = 3 };

  enum TDriveType { NONE = 0, WITHOUT_DISKCHANGE, WITH_DISKCHANGE, HARDDISK };

  typedef struct TFormBuffer {
    byte cylinder;
    byte side;
    byte sector;
    byte BytesPerSector;
  } *PFormBuffer;

  enum TFloppyType { DD = 1, DD_IN_HD = 2, HD = 3 };

             TFloppyDisk  ( TDrive aDrive                             );
  bool       Init         ( void                                      );
  TError     GetError     ( void                                      );
  bool       ReadSector   ( PBuffer buffer, int side,
                            int cylinder,   int sector, int count = 1 );
  bool       WriteSector  ( PBuffer buffer, int side,
                            int cylinder,   int sector, int count = 1 );
  bool       VerifySector ( PBuffer buffer, int side,
                            int cylinder,   int sector, int count = 1 );
  bool       Format1Track ( PFormBuffer buffer, int side,
                            int cylinder, int count                   );
  bool       MoveHead     ( int side, int cylinder                    );
  TDriveType DetectType   ( void                                      );
  bool       DiskChanged  ( void                                      );
  bool       FixDiskType  ( TFloppyType type                          );
  bool       SetMedia     ( int cylinder, int sector                  );
};

inline TFloppyDisk::TFloppyDisk( TDrive aDrive ) {
  driveno    = aDrive;
  driveerror = NO_ERROR;
}

inline TFloppyDisk::TError TFloppyDisk::GetError( void ) {
  return driveerror;
}

#endif /* #ifndef __SRT_FDISK_HH__ */
