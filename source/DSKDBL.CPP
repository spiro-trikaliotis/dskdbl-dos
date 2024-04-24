static char RCSID[] = "$Id: DSKDBL.CPP,v 1.2 2005/01/10 10:47:37 trikalio Exp $";

#pragma hdrfile "dskdbl.sym"
#include <fdisk.hh>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include <iostream.h>
#include <fstream.h>

#undef _LIBC
#define __GNU_LIBRARY__ 1
#include "getopt.h"

#pragma hdrstop

// the boot block for writing after formatting/clearing
static byte BootBlock[512] = {
#include "boot.out"
   };

struct BootInfo {
  char reserved0[3]; // jump to the loader
  char reserved1[8]; // Marker for OS (i.e., 'MSDOS 5.0')
  int  BytePerSctr;  // db      512;    ; bytes per sector
  byte SctrPerClstr; // db      1       ; sectors per cluster
  int  ReservedSctr; // dw      1       ; no of reserved sectors
  byte NumberFATs;   // db      2       ; no of FATs
  int  NoDirEntries; // dw      0E0h    ; no of directory entries
  int  NoSectors;    // dw      0b40h   ; no of sectors in volume
  byte MediaDesc;    // db      0F0h    ; media descriptor
  int  SctrPerFAT;   // dw      9       ; no of sectors per FAT
  int  SctrPerTrack; // dw      18      ; no of sectors per track
  int  NoHeads;      // dw      2       ; no of heads

  long NoFirstSctr;  // dd      0       ; no of first sector in volume (low word)
  int  reserved2;    // dw      0       ; location 020h (???)
  int  reserved3;    // dw      0       ; location 022h (???)

  byte NoOfDisk;     // db      0h      ; disk number for access via INT 13h
  byte reserved4;    // db      01h     ; location 025h (???)
  byte SerNoMark;    // db      0h      ; marker: serial-no is valid (=29h)
  int  SerNoLow;     // dw      0000h   ; serial-no (lower)
  int  SerNoHigh;    // dw      0000h   ; serial-no (higher)
};

long gethex( char *arg, bool &ok ) {
  int i;
  long ret;

  ok = true;

  for (i=8; i; i--) {
    ret <<= 4;
    if ((*arg >= '0') && (*arg <= '9'))
      ret += *arg - '0';
    else
      if ((*arg >= 'A') && (*arg <= 'F'))
        ret += *arg - 'A'+10;
      else
        if ((*arg >= 'a') && (*arg <= 'f'))
          ret += *arg - 'a'+10;
        else {
          cerr << ( (*arg)
                    ? "unknown hex-digit"
                    : "hex value not long enough"
                  )
               << (char) *arg
               << endl;
          ok = false;
          break;
        }

    arg++;
  }

  if (*arg && ok) {
    cerr << "Hex value too long\n";
    ok = false;
  }

  return ret;
}

void DoBanner( void ) {
  static bool DoneBanner = false;

  if (!DoneBanner) {
    DoneBanner = true;
    cerr << "Disk-Doubler (c) 1996 Spiro Trikaliotis\n";
  }
}

void DoHelp( char name[] ) {
  static bool DoneHelp = false;

  if (!DoneHelp) {
    DoneHelp = true;

    DoBanner();

    char *p     = name;
    char *pName = name;

    while (*p)
      if (*p++=='\\')
        pName = p;

    p = pName;
    while (*p)
      if (*p++=='.')
        *--p = 0;

    cerr << "Usage: " << pName << " [-?bvrwdfFCI] filename \n\n"
            "Available options:\n\n"
            "-?          Print this help screen\n"
            "-b          Print out banner\n"
            "-v          Verbose mode\n"
            "\n"
            "-r          Read a complete disk into a file\n"
            "-w          Write a complete disk from a file\n"
            "-c          Compare a disk with a file\n"
            "-d:{x}      Work on drive x (default=A:)\n"
            "-f:360      Diskformat:  360K\n"
            "-f:1200     Diskformat: 1200K\n"
            "-f:720      Diskformat:  720K\n"
            "-f:1440     Diskformat: 1440K\n"
            "-F          Really FORMAT the disk to be written\n"
            "-C          Really CLEAR a disk\n"
            "-I          Initialize the disk (i.e., rewrite boot/root/FAT)\n"
            "-Nxxxxyyyy  Assign the volume serial no. xxxx-yyyy to the disk\n"
            "            (only with -F, -C, -I)\n"
            "-B          Write a new boot sector\n"
            "\n"
            "<ESC> aborts immediately"
         << endl;
  }
}

bool GetParameter( int nFormat, int& tracks, int& sectors ) {
  bool RetCode = true;

  switch (nFormat) {
    case  360: tracks = 40; sectors =  9; break;
    case  720: tracks = 80; sectors =  9; break;
    case 1200: tracks = 80; sectors = 15; break;
    case 1440: tracks = 80; sectors = 18; break;
    case 2880: tracks = 80; sectors = 36; break;

    default: RetCode = false; break;
  }

  return RetCode;
}


void Compare( int track, int sectors, char VerifyBuffer[], char DiskBuffer[] ) {
  for (int sector = 1; sector <= sectors; sector++) {
    char *pVerify = &VerifyBuffer[512*(sector-1)];
    char *pDisk   = &DiskBuffer  [512*(sector-1)];

    for (int i=0; i<=511; i++) {
      if (*pVerify++ != *pDisk++) {
        cout << "Track "
             << track
             << ", sector "
             << sector
             << " is different!\n";
        break;
      }
    }
  }
}

bool DoFormatOneTrack( TFloppyDisk *FormatDisk, bool ReallyFormat, int sectors, int track, int head ) {
  bool ok = false;

  if (ReallyFormat) {
    TFloppyDisk::TFormBuffer *FormBuffer = new TFloppyDisk::TFormBuffer[sectors];
    TFloppyDisk::TFormBuffer *buf        = FormBuffer;
    if (buf) {
      for (byte sector=1;sector<=sectors;sector++) {
        buf->cylinder       = track;
        buf->side           = head;
        buf->sector         = sector;
        buf->BytesPerSector = TFloppyDisk::B512;
        buf++;
      }
      ok = FormatDisk->Format1Track ( FormBuffer, head, track, sectors )
           ? false
           : true;

      if (!ok)
        cerr << "Error formatting track "
             << track
             << ", side "
             << head
             << "."
             << endl;

      delete[] FormBuffer;
    }
    else
      cerr << "Not enough memory to format track!" << endl;
  }
  else {
    char *nulltrack = new byte[sectors*512];
    char *p = nulltrack;
    if (p) { // clear the null-track
      for (int i=sectors*512-1; i>=0; i--)
      *p++ = 0xF6;

      ok = FormatDisk->WriteSector( nulltrack, head, track, 1, sectors )
           ? false
           : true;

      if (!ok)
        cerr << "Error clearing track "
             << track
             << ", side "
             << head
             << "."
             << endl;

      delete[] nulltrack;
    }
    else
      cerr << "Not enough memory to clear track!" << endl;
  }
  return ok;
}

// Variables to remember given options
bool bError      = false;
bool bVerbose    = false;
bool bRead       = false;
bool bVerify     = false;
bool bWrite      = false;
bool bFormat     = false;
bool bClear      = false;
bool bInit       = false;
int  nDrive      = -1;
int  nFormat     = -1;
bool bBootSector = false;
bool bVSN        = false;
long lVSN        = 0;

bool DoVerify( char *filename ) {
  ifstream file(filename,ios::binary|ios::nocreate);
  bool RetCode = false;

  if (file.bad()) {
    cerr << "File " << filename << " does not exist!\n";
  }
  else {
    int tracks;
    int sectors;

    if (GetParameter( nFormat, tracks, sectors )) {
      TFloppyDisk  VerifyDisk( (TFloppyDisk::TDrive) nDrive );
      char        *VerifyBuffer = new char[sectors*512];
      char        *DiskBuffer   = new char[sectors*512];

      bool ok = true;

      // try 1st read:
      if (VerifyDisk.ReadSector( VerifyBuffer, 0, 1, 1 ))
        VerifyDisk.Init();  // if neccessary, initialize disk

      for (int track=0; (track<tracks) && ok; track++) {
        for (int side=0; (side<=1) && ok; side++) {
          file.read(VerifyBuffer, sectors*512);
          if (file.bad()) {
            cerr << "Error while reading input file!\n";
            ok = false;
          }
          else {
            if (VerifyDisk.ReadSector( DiskBuffer, side, track, 1, sectors )) {
              cerr << "Error reading side "
                   << side
                   << ", track "
                   << track
                   << endl;

              ok = false;
            }
          }
          Compare( track, sectors, VerifyBuffer, DiskBuffer );

          if (kbhit())
            switch (getch()) {
              case 0:
                getch();
                break;

              case 27:
                cerr << "Aborting due to user intervention" << endl;
                ok = false;
            }
        }
      }

      RetCode = ok;
      delete[] DiskBuffer;
      delete[] VerifyBuffer;
    }
    else
      cerr << "Internal error: DoVerify, GetParameter( " << nDrive << " );\n";

    file.close();
  }
  return RetCode;
}

bool DoWrite( char *filename ) {
  ifstream file(filename,ios::binary|ios::nocreate);
  bool RetCode = false;

  if (file.bad()) {
    cerr << "File " << filename << " does not exist!\n";
  }
  else {
    int tracks;
    int sectors;

    if (GetParameter( nFormat, tracks, sectors )) {
      TFloppyDisk  WriteDisk( (TFloppyDisk::TDrive) nDrive );
      char        *WriteBuffer = new char[sectors*512];

      bool ok = true;

      if (bFormat)
        WriteDisk.SetMedia( tracks, sectors );
      else
        // try 1st read:
        if (WriteDisk.ReadSector( WriteBuffer, 0, 1, 1 ))
          WriteDisk.Init();  // if neccessary, initialize disk

      for (int track=0; (track<tracks) && ok; track++) {
        for (int side=0; (side<=1) && ok; side++) {
          if (bFormat)
            ok = DoFormatOneTrack( &WriteDisk, true, sectors, track, side );

          if (ok) {
            file.read(WriteBuffer, sectors*512);
            if (file.bad()) {
              cerr << "Error while reading input file!\n";
              ok = false;
            }
            else {
              //
              if ((track==0) && (side==0)) {
                // here is the boot-sector
                if (bVSN)
                  if (!bBootSector) {
                    if (WriteBuffer[0x26] != 0x29) {
                      cerr << "Um die Volume serial no. zu setzen, muá ich den Bootsektor neu schreiben!" << endl;
                      bVSN = false;
                    }
                  }

                if (bBootSector) {
                  byte *pW = WriteBuffer;
                  byte *pR = BootBlock;
                  for (int i=0; i<11; i++)
                    *pW++ = *pR++;
                  for (; i<26; i++)
                    pW++, pR++;
                  for (; i<512; i++)
                    *pW++ = *pR++;

                }
                if (bVSN) {
                  WriteBuffer[0x26] = 0x29; // Serial no. is valid!
                  long *p = (long*) &WriteBuffer[0x27];
                  *p      = lVSN;
                }
              }
              if (WriteDisk.WriteSector( WriteBuffer, side, track, 1, sectors )) {
                cerr << "Error writing side "
                     << side
                     << ", track "
                     << track
                     << endl;

                ok = false;
              }
            }
          }

          if (kbhit())
            switch (getch()) {
              case 0:
                getch();
                break;

              case 27:
                cerr << "Aborting due to user intervention" << endl;
                ok = false;
            }
        }
      }

      RetCode = ok;
      delete[] WriteBuffer;
    }
    else
      cerr << "Internal error: DoWrite, GetParameter( " << nDrive << " );\n";

    file.close();
  }
  return RetCode;
}

bool DoRead( char *filename ) {
  ofstream file(filename,ios::binary|ios::noreplace);
  bool RetCode = false;

  if (file.bad()) {
    cerr << "File " << filename << " already exists. Shall I overwrite it? (y/N)";
    char ch = getch();
    switch (ch) {
      case 'y': case 'Y':
        cerr << "Yes\n";
        file.close();
        file.open(filename,ios::binary);
        break;

      case 0:
        getch();

      default:
        cerr << "No\n";
    }
  }

  if (!file.bad()) {
    int tracks;
    int sectors;

    if (GetParameter( nFormat, tracks, sectors )) {
      TFloppyDisk  ReadDisk( (TFloppyDisk::TDrive) nDrive );
      char        *ReadBuffer = new char[sectors*512];

      bool ok = true;

      // try 1st read (so, clear the "disk changed" error message)
      if (ReadDisk.ReadSector( ReadBuffer, 0, 1, 1 ))
        ReadDisk.Init();  // if neccessary, initialize disk

      for (int track=0; (track<tracks) && ok; track++) {
        for (int side=0; (side<=1) && ok; side++) {

          if (ReadDisk.ReadSector( ReadBuffer, side, track, 1, sectors )) {
            cerr << "Error reading side "
                 << side
                 << ", track "
                 << track
                 << endl;

            ok = false;
          }
          else {
            file.write(ReadBuffer, sectors*512);
            if (file.bad()) {
              cerr << "Error while writing output file!\n";
              ok = false;
            }
          }

          if (kbhit())
            switch (getch()) {
              case 0:
                getch();
                break;

              case 27:
                cerr << "Aborting due to user intervention" << endl;
                ok = false;
            }

        }
      }

      RetCode = ok;
      delete[] ReadBuffer;
    }
    else
      cerr << "Internal error: DoRead, GetParameter( " << nDrive << " );\n";

    file.close();
  }
  return RetCode;
}

bool DoFormat( bool ReallyFormat ) {
  int track, head;

  bool ok = true;

  int tracks;
  int sectors;

  if (GetParameter( nFormat, tracks, sectors )) {
    TFloppyDisk FormatDisk( (TFloppyDisk::TDrive) nDrive );
    FormatDisk.SetMedia( tracks, sectors );

    bool Reset = true;

    // Durch das Z„hlen von 79 auf 0 spare ich mir ein groáes Seek!
    for (track=tracks-1;(track>=0) && ok;track--)
      for (head=1;(head>=0) && ok;head--) {
        ok = DoFormatOneTrack( &FormatDisk, ReallyFormat, sectors, track, head );

        if (!ok)
          if (Reset) {
            FormatDisk.Init();
            FormatDisk.SetMedia( tracks, sectors );
            ok = DoFormatOneTrack( &FormatDisk, ReallyFormat, sectors, track, head );
            Reset = false;
          }

        if (kbhit())
          switch (getch()) {
            case 0:
              getch();
              break;

            case 27:
              cerr << "Aborting due to user intervention" << endl;
              ok = false;
          }
      }
  }
  return ok;
}

// write boot block & FATs & root
bool DoInit( void ) {
  int tracks;
  int sectors;
  bool ok = true;

  if (GetParameter( nFormat, tracks, sectors )) {
    TFloppyDisk  InitDisk( (TFloppyDisk::TDrive) nDrive );
    InitDisk.SetMedia( tracks, sectors );

    char far *NullBlock = new char[sectors*512];

    char *p = NullBlock;
    if (p) { // clear the null-track
      // try 1st read (so, clear the "disk changed" error message)
      if (InitDisk.ReadSector( NullBlock, 0, 1, 1 ))
        InitDisk.Init();  // if neccessary, initialize disk

      for (int i=sectors*512-1; i>=0; i--)
        *p++ = 0;

      ok = true;
    }
    else
      ok = false;

    // write the boot block
    if (ok)
      ok = InitDisk.WriteSector( BootBlock, 0, 0, 1 )
           ? false
           : true;
    else
      cerr << "boot block" << endl;

    // clear head 0 track 0 (excluding sector 1, the boot block!)
    if (ok) {
      if (bVSN) {
        BootBlock[0x26] = 0x29; // Serial no. is valid!
        long *p = (long*) &BootBlock[0x27];
        *p = lVSN;
      }

      ok = InitDisk.WriteSector( NullBlock, 0, 0, 2, sectors-1 )
           ? false
           : true;
    }

    // clear head 1 track 0
    if (ok)
      ok = InitDisk.WriteSector( NullBlock, 1, 0, 1, sectors )
           ? false
           : true;

    // write in FAT: cluster 2 is used
    NullBlock[0] = 0xF0;
    NullBlock[1] = 0xFF;
    NullBlock[2] = 0xFF;

    // Write FAT 1
    if (ok)
      ok = InitDisk.WriteSector( NullBlock, 0, 0, 2, 1 )
           ? false
           : true;

    // Write FAT 2
    if (ok)
      ok = InitDisk.WriteSector( NullBlock, 0, 0, 11, 1 )
           ? false
           : true;

    delete[] NullBlock;
  }
  return ok;
}

bool ProcessFile( char *filename, bool WaitForKey = false ) {
  char ch = 0;

  if (WaitForKey) {
    cerr << "--- Press <SPACE> or <ENTER> for proceeding, "
            "or <ESC> for aborting ---"
         << endl;

    do {
      ch = getch();
      cout << (int) ch << endl;
      switch (ch) {
        case 32:   // Make <SPACE> synonymous to <ENTER>
        case 10:   // Make <LF>    synonymous to <ENTER>
          ch = 13;
          break;

        case 0:
          getch();
          break;
      }
    } while ((ch!=13) && (ch!=27));

  }

  if (bVerbose)
    cerr << filename
         << endl;

  bool RetCode = false;

  if (ch!=27) {
    if (bRead)
      RetCode = DoRead(filename);
    else
      if (bWrite)
        RetCode = DoWrite(filename);

    if (bVerify)
      RetCode = DoVerify(filename);
  }
  else
    cerr << "Aborting due to user intervention!\n";

  return RetCode;
}

int main( int argc, char *const *argv ) {
  int c;

  while (1) {
    c = getopt (argc, argv, "?bvrwtd:f:FCIN:B");
    if (c == EOF)
      break;

    switch (c) {
      case '?':
        DoHelp(argv[0]);
        bError = true;
        break;

      case 'b':
        DoBanner();
        break;

      case 'v':
        if (!bVerbose)
          bVerbose = true;
        else {
          cerr << "Option -" << (char) c << " specified multiple times" << endl;
          bError = true;
        }
        break;

      case 'r':
        if (!bRead) {
          if (!bWrite)
            bRead = true;
          else {
            cerr << "Read and write cannot be used concurrently!" << endl;
            bError = true;
          }
        }
        else {
          cerr << "Option -" << (char) c << " specified multiple times" << endl;
          bError = true;
        }
        break;

      case 'w':
        if (!bWrite) {
          if (!bRead)
            bWrite = true;
          else {
            cerr << "Read and write cannot be used concurrently!" << endl;
            bError = true;
          }
        }
        else {
          cerr << "Option -" << (char) c << " specified multiple times" << endl;
          bError = true;
        }
        break;

      case 'c':
        if (!bVerify)
          bVerify = true;
        else {
          cerr << "Option -" << (char) c << " specified multiple times" << endl;
          bError = true;
        }
        break;

      case 'F':
        if (!bFormat)
          bFormat = true;
        else {
          cerr << "Option -" << (char) c << " specified multiple times" << endl;
          bError = true;
        }
        break;

      case 'C':
        if (!bClear) {
          if ( !bFormat && !bRead && !bWrite )
            bClear = true;
          else
            cerr << "Clearing cannot be used concurrently to read, write, or format!" << endl;
        }
        else {
          cerr << "Option -" << (char) c << " specified multiple times" << endl;
          bError = true;
        }
        break;

      case 'I':
        if (!bInit) {
          if (bRead || bWrite) {
            cerr << "Initializing cannot be used concurrently to read or write" << endl;
            bError = true;
          }
          else
            bInit = true;
        }
        else {
          cerr << "Option -" << (char) c << " specified multiple times" << endl;
          bError = true;
        }
        break;

      case 'd':
        if (optarg) {
          if (nDrive>=0) {
            cerr << "Option -d specified multiple times" << endl;
            bError = true;
            break;
          }

          if ( (      ((optarg[2]==0) && (optarg[1]==':'))
                   || ((optarg[1]==0)
               )
               && optarg[0]!=0)
             )
          { switch (optarg[0]) {
              case 'a': case 'A': case '0':
                nDrive = 0;
                break;

              case 'b': case 'B': case '1':
                nDrive = 1;
                break;

              default:
                cerr << "Please enter drive letter with option -d" << endl;
                bError = true;
            }
          }
          else {
            cerr << "Please enter drive letter with option -d" << endl;
            bError = true;
          }
        }
        else {
          cerr << "Please specify drive with -d option" << endl;
          bError = true;
        }
        break;

      case 'f':
        if (!optarg) {
          cerr << "Please specify format with -f option" << endl;
          bError = true;
        }
        else {
          if (nFormat>=0) {
            cerr << "Option -f specified multiple times" << endl;
            bError = true;
            break;
          }

          if ((optarg[0]=='3') && (optarg[1]=='6'))
            nFormat = 360;
          else {
            if ((optarg[0]=='7') && (optarg[1]=='2'))
              nFormat = 720;
            else {
              if ((optarg[0]=='1') && (optarg[1]=='2'))
                nFormat = 1200;
              else {
                if ((optarg[0]=='1') && (optarg[1]=='4'))
                  nFormat = 1440;
                else {
                  if ((optarg[0]=='2') && (optarg[1]=='8')) {
                    nFormat = 2880;
                    cerr << "Warning: Format 2880 KB is not officially implemented!" << endl;
                  }
                  else {
                    cerr << "Format must be 360, 720, 1200, or 1440!" << endl;
                    bError = true;
                  }
                }
              }
            }
          }
        }
        break;

      case 'N':
        if (!bVSN) {
          bool ok;
          lVSN = gethex(optarg, ok);
          bVSN = ok;

          if (!ok)
            bError = true;
        }
        else {
          cerr << "Option -" << (char) c << " specified multiple times" << endl;
          bError = true;
        }
        break;

      case 'B':
        if (!bBootSector)
          bBootSector = true;
        else {
          cerr << "Option -" << (char) c << " specified multiple times" << endl;
          bError = true;
        }
        break;

      default:
        cerr << "Unknow option -" << (char) c << endl;
        bError = true;
    }
  }

  bool rProcessFile = false;

  if (!bError) {
    if (!bRead && !bWrite && !bVerify && !bFormat && !bClear && !bInit)
      cerr << "At least one of -r, -w, -c, -F, -C or -I has to be specified" << endl;
    else {
      if (nDrive<0) {
        nDrive = 0;
        if (bVerbose)
          cerr << "Drive not specified. Assuming A:." << endl;
      }

      if (nFormat<0) {
        nFormat = 1440;
        if (bVerbose)
          cerr << "Format not specified. Assuming 1440K." << endl;
      }

      if (optind < argc) {

        if (bVerbose)
          cout << "\nProcessing files:" << endl;

        if (optind < argc)
          rProcessFile = ProcessFile( argv[optind++]       );

        while ((optind < argc) && rProcessFile)
          rProcessFile = ProcessFile( argv[optind++], true );

        cerr << (  (rProcessFile == true)
                   ? "Disk(s) successfully processed"
                   : "Error processing disk(s)!"
                )
             << endl;
      }
      else {
        if (nFormat!=1440)
          cerr << "In this beta, I can only format 1.44 MB. I'm sorry.\n";
        else {
          if (!bFormat && !bClear && !bInit)
            cerr << "\n\nNo file for processing specified!" << endl;
          else {
            if (bFormat || bClear)
              rProcessFile = DoFormat( bFormat ); // Format or clear, as specified!

            if (rProcessFile || bInit)
              rProcessFile = DoInit();

            cerr << ( (rProcessFile == true)
                      ? "Disk successfully "
                      : "Error "
                    )
                 << ( (bFormat == true)
                      ? "formatt"
                      : (bClear == true )
                        ? "clear"
                        : "initializ"
                    )
                 << ( (rProcessFile == true)
                      ? "ed."
                      : "ing disk."
                    );
          }
        }
      }
    }
  }


  return 0;
}

