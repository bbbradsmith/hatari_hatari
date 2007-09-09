/*
  Hatari - vdi.c

  This file is distributed under the GNU Public License, version 2 or at
  your option any later version. Read the file gpl.txt for details.

  VDI (Virtual Device Interface) (Trap #2)

  To get higher resolutions on the Desktop, we intercept the VDI/Line-A calls
  and set elements in their structures to the higher width/height/cel/planes.
  We need to intercept the initial Line-A call (which we force into the TOS on
  boot-up) and also the init calls to the VDI.
*/
const char VDI_rcsid[] = "Hatari $Id: vdi.c,v 1.23 2007-09-09 20:49:59 thothy Exp $";

#include "main.h"
#include "file.h"
#include "gemdos.h"
#include "m68000.h"
#include "screen.h"
#include "stMemory.h"
#include "vdi.h"
#include "video.h"
#include "configuration.h"


Uint32 VDI_OldPC;                  /* When call Trap#2, store off PC */

BOOL bUseVDIRes = FALSE;           /* Set to TRUE (if want VDI), or FALSE (ie for games) */
/* defaults */
int VDIRes = 0;                    /* 0,1 or 2 (low, medium, high) */
int VDIWidth = 640;                /* 640x480, 800x600 or 1024x768 */
int VDIHeight = 480;
int VDIPlanes = 4;
static int VDIColours = 16;
static int VDICharHeight = 8;

static Uint32 LineABase;           /* Line-A structure */
static Uint32 FontBase;            /* Font base, used for 16-pixel high font */

static Uint32 Control;
static Uint32 Intin;
static Uint32 Ptsin;
static Uint32 Intout;
static Uint32 Ptsout;


/*-----------------------------------------------------------------------*/
/* Desktop TOS 1.04 and TOS 2.06 desktop configuration files */
static const Uint8 DesktopScript[504] =
{
0x23,0x61,0x30,0x30,0x30,0x30,0x30,0x30,0x0D,0x0A,0x23,0x62,0x30,0x30,0x30,0x30,0x30,0x30,0x0D,0x0A,0x23,0x63,0x37,0x37,0x37,0x30,0x30,0x30,0x37,0x30,0x30,0x30,
0x36,0x30,0x30,0x30,0x37,0x30,0x30,0x35,0x35,0x32,0x30,0x30,0x35,0x30,0x35,0x35,0x35,0x32,0x32,0x32,0x30,0x37,0x37,0x30,0x35,0x35,0x37,0x30,0x37,0x35,0x30,0x35,
0x35,0x35,0x30,0x37,0x37,0x30,0x33,0x31,0x31,0x31,0x31,0x30,0x33,0x0D,0x0A,0x23,0x64,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x0D,0x0A,
0x23,0x45,0x20,0x31,0x38,0x20,0x31,0x31,0x20,0x0D,0x0A,0x23,0x57,0x20,0x30,0x30,0x20,0x30,0x30,0x20,0x30,0x30,0x20,0x30,0x37,0x20,0x32,0x36,0x20,0x30,0x43,0x20,
0x30,0x30,0x20,0x40,0x0D,0x0A,0x23,0x57,0x20,0x30,0x30,0x20,0x30,0x30,0x20,0x30,0x32,0x20,0x30,0x42,0x20,0x32,0x36,0x20,0x30,0x39,0x20,0x30,0x30,0x20,0x40,0x0D,
0x0A,0x23,0x57,0x20,0x30,0x30,0x20,0x30,0x30,0x20,0x30,0x41,0x20,0x30,0x46,0x20,0x31,0x41,0x20,0x30,0x39,0x20,0x30,0x30,0x20,0x40,0x0D,0x0A,0x23,0x57,0x20,0x30,
0x30,0x20,0x30,0x30,0x20,0x30,0x45,0x20,0x30,0x31,0x20,0x31,0x41,0x20,0x30,0x39,0x20,0x30,0x30,0x20,0x40,0x0D,0x0A,0x23,0x4D,0x20,0x30,0x31,0x20,0x30,0x30,0x20,
0x30,0x30,0x20,0x46,0x46,0x20,0x43,0x20,0x48,0x41,0x52,0x44,0x20,0x44,0x49,0x53,0x4B,0x40,0x20,0x40,0x20,0x0D,0x0A,0x23,0x4D,0x20,0x30,0x30,0x20,0x30,0x30,0x20,
0x30,0x30,0x20,0x46,0x46,0x20,0x41,0x20,0x46,0x4C,0x4F,0x50,0x50,0x59,0x20,0x44,0x49,0x53,0x4B,0x40,0x20,0x40,0x20,0x0D,0x0A,0x23,0x4D,0x20,0x30,0x30,0x20,0x30,
0x31,0x20,0x30,0x30,0x20,0x46,0x46,0x20,0x42,0x20,0x46,0x4C,0x4F,0x50,0x50,0x59,0x20,0x44,0x49,0x53,0x4B,0x40,0x20,0x40,0x20,0x0D,0x0A,0x23,0x54,0x20,0x30,0x30,
0x20,0x30,0x33,0x20,0x30,0x32,0x20,0x46,0x46,0x20,0x20,0x20,0x54,0x52,0x41,0x53,0x48,0x40,0x20,0x40,0x20,0x0D,0x0A,0x23,0x46,0x20,0x46,0x46,0x20,0x30,0x34,0x20,
0x20,0x20,0x40,0x20,0x2A,0x2E,0x2A,0x40,0x20,0x0D,0x0A,0x23,0x44,0x20,0x46,0x46,0x20,0x30,0x31,0x20,0x20,0x20,0x40,0x20,0x2A,0x2E,0x2A,0x40,0x20,0x0D,0x0A,0x23,
0x47,0x20,0x30,0x33,0x20,0x46,0x46,0x20,0x20,0x20,0x2A,0x2E,0x41,0x50,0x50,0x40,0x20,0x40,0x20,0x0D,0x0A,0x23,0x47,0x20,0x30,0x33,0x20,0x46,0x46,0x20,0x20,0x20,
0x2A,0x2E,0x50,0x52,0x47,0x40,0x20,0x40,0x20,0x0D,0x0A,0x23,0x50,0x20,0x30,0x33,0x20,0x46,0x46,0x20,0x20,0x20,0x2A,0x2E,0x54,0x54,0x50,0x40,0x20,0x40,0x20,0x0D,
0x0A,0x23,0x46,0x20,0x30,0x33,0x20,0x30,0x34,0x20,0x20,0x20,0x2A,0x2E,0x54,0x4F,0x53,0x40,0x20,0x40,0x20,0x0D,0x0A,0x1A,
};

static const Uint8 NewDeskScript[786] =
{
0x23,0x61,0x30,0x30,0x30,0x30,0x30,0x30,0x0D,0x0A,0x23,0x62,0x30,0x30,0x30,0x30,0x30,0x30,0x0D,0x0A,0x23,0x63,0x37,0x37,0x37,0x30,0x30,0x30,0x37,0x30,0x30,0x30,
0x36,0x30,0x30,0x30,0x37,0x30,0x30,0x35,0x35,0x32,0x30,0x30,0x35,0x30,0x35,0x35,0x35,0x32,0x32,0x32,0x30,0x37,0x37,0x30,0x35,0x35,0x37,0x30,0x37,0x35,0x30,0x35,
0x35,0x35,0x30,0x37,0x37,0x30,0x33,0x31,0x31,0x31,0x31,0x30,0x33,0x0D,0x0A,0x23,0x64,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x0D,0x0A,
0x23,0x4B,0x20,0x34,0x46,0x20,0x35,0x33,0x20,0x34,0x43,0x20,0x30,0x30,0x20,0x34,0x36,0x20,0x34,0x32,0x20,0x34,0x33,0x20,0x35,0x37,0x20,0x34,0x35,0x20,0x35,0x38,
0x20,0x30,0x30,0x20,0x30,0x30,0x20,0x30,0x30,0x20,0x30,0x30,0x20,0x30,0x30,0x20,0x30,0x30,0x20,0x30,0x30,0x20,0x30,0x30,0x20,0x30,0x30,0x20,0x30,0x30,0x20,0x30,
0x30,0x20,0x30,0x30,0x20,0x30,0x30,0x20,0x35,0x32,0x20,0x30,0x30,0x20,0x30,0x30,0x20,0x34,0x44,0x20,0x35,0x36,0x20,0x35,0x30,0x20,0x30,0x30,0x20,0x40,0x0D,0x0A,
0x23,0x45,0x20,0x31,0x38,0x20,0x30,0x31,0x20,0x30,0x30,0x20,0x30,0x36,0x20,0x0D,0x0A,0x23,0x51,0x20,0x34,0x31,0x20,0x34,0x30,0x20,0x34,0x33,0x20,0x34,0x30,0x20,
0x34,0x33,0x20,0x34,0x30,0x20,0x0D,0x0A,0x23,0x57,0x20,0x30,0x30,0x20,0x30,0x30,0x20,0x30,0x30,0x20,0x30,0x37,0x20,0x32,0x36,0x20,0x30,0x43,0x20,0x30,0x30,0x20,
0x40,0x0D,0x0A,0x23,0x57,0x20,0x30,0x30,0x20,0x30,0x30,0x20,0x30,0x32,0x20,0x30,0x42,0x20,0x32,0x36,0x20,0x30,0x39,0x20,0x30,0x30,0x20,0x40,0x0D,0x0A,0x23,0x57,
0x20,0x30,0x30,0x20,0x30,0x30,0x20,0x30,0x41,0x20,0x30,0x46,0x20,0x31,0x41,0x20,0x30,0x39,0x20,0x30,0x30,0x20,0x40,0x0D,0x0A,0x23,0x57,0x20,0x30,0x30,0x20,0x30,
0x30,0x20,0x30,0x45,0x20,0x30,0x31,0x20,0x31,0x41,0x20,0x30,0x39,0x20,0x30,0x30,0x20,0x40,0x0D,0x0A,0x23,0x57,0x20,0x30,0x30,0x20,0x30,0x30,0x20,0x30,0x34,0x20,
0x30,0x37,0x20,0x32,0x36,0x20,0x30,0x43,0x20,0x30,0x30,0x20,0x40,0x0D,0x0A,0x23,0x57,0x20,0x30,0x30,0x20,0x30,0x30,0x20,0x30,0x43,0x20,0x30,0x42,0x20,0x32,0x36,
0x20,0x30,0x39,0x20,0x30,0x30,0x20,0x40,0x0D,0x0A,0x23,0x57,0x20,0x30,0x30,0x20,0x30,0x30,0x20,0x30,0x38,0x20,0x30,0x46,0x20,0x31,0x41,0x20,0x30,0x39,0x20,0x30,
0x30,0x20,0x40,0x0D,0x0A,0x23,0x57,0x20,0x30,0x30,0x20,0x30,0x30,0x20,0x30,0x36,0x20,0x30,0x31,0x20,0x31,0x41,0x20,0x30,0x39,0x20,0x30,0x30,0x20,0x40,0x0D,0x0A,
0x23,0x4E,0x20,0x46,0x46,0x20,0x30,0x34,0x20,0x30,0x30,0x30,0x20,0x40,0x20,0x2A,0x2E,0x2A,0x40,0x20,0x40,0x20,0x0D,0x0A,0x23,0x44,0x20,0x46,0x46,0x20,0x30,0x31,
0x20,0x30,0x30,0x30,0x20,0x40,0x20,0x2A,0x2E,0x2A,0x40,0x20,0x40,0x20,0x0D,0x0A,0x23,0x47,0x20,0x30,0x33,0x20,0x46,0x46,0x20,0x30,0x30,0x30,0x20,0x2A,0x2E,0x41,
0x50,0x50,0x40,0x20,0x40,0x20,0x40,0x20,0x0D,0x0A,0x23,0x47,0x20,0x30,0x33,0x20,0x46,0x46,0x20,0x30,0x30,0x30,0x20,0x2A,0x2E,0x50,0x52,0x47,0x40,0x20,0x40,0x20,
0x40,0x20,0x0D,0x0A,0x23,0x59,0x20,0x30,0x33,0x20,0x46,0x46,0x20,0x30,0x30,0x30,0x20,0x2A,0x2E,0x47,0x54,0x50,0x40,0x20,0x40,0x20,0x40,0x20,0x0D,0x0A,0x23,0x50,
0x20,0x30,0x33,0x20,0x46,0x46,0x20,0x30,0x30,0x30,0x20,0x2A,0x2E,0x54,0x54,0x50,0x40,0x20,0x40,0x20,0x40,0x20,0x0D,0x0A,0x23,0x46,0x20,0x30,0x33,0x20,0x30,0x34,
0x20,0x30,0x30,0x30,0x20,0x2A,0x2E,0x54,0x4F,0x53,0x40,0x20,0x40,0x20,0x40,0x20,0x0D,0x0A,0x23,0x4D,0x20,0x30,0x30,0x20,0x30,0x31,0x20,0x30,0x30,0x20,0x46,0x46,
0x20,0x43,0x20,0x48,0x41,0x52,0x44,0x20,0x44,0x49,0x53,0x4B,0x40,0x20,0x40,0x20,0x0D,0x0A,0x23,0x4D,0x20,0x30,0x30,0x20,0x30,0x30,0x20,0x30,0x30,0x20,0x46,0x46,
0x20,0x41,0x20,0x46,0x4C,0x4F,0x50,0x50,0x59,0x20,0x44,0x49,0x53,0x4B,0x40,0x20,0x40,0x20,0x0D,0x0A,0x23,0x4D,0x20,0x30,0x31,0x20,0x30,0x30,0x20,0x30,0x30,0x20,
0x46,0x46,0x20,0x42,0x20,0x46,0x4C,0x4F,0x50,0x50,0x59,0x20,0x44,0x49,0x53,0x4B,0x40,0x20,0x40,0x20,0x0D,0x0A,0x23,0x54,0x20,0x30,0x30,0x20,0x30,0x33,0x20,0x30,
0x32,0x20,0x46,0x46,0x20,0x20,0x20,0x54,0x52,0x41,0x53,0x48,0x40,0x20,0x40,0x20,0x0D,0x0A,
};


/*-----------------------------------------------------------------------*/
/**
 * Returns given value after constraining it within "min" and "max" values
 * and making it evenly divisable by "align"
 */
static int limit(int value, int align, int min, int max)
{
	if (value < min) {
		value = min;
	}
	if (value > max) {
		value = max;
	}
	value = (value/align)*align;
	return value;
}


/*-----------------------------------------------------------------------*/
/**
 * Set Width/Height/BitDepth according to passed GEMRES_640x480,
 * GEMRES_800x600, GEMRES_OTHER. Align size when necessary.
 */
void VDI_SetResolution(int GEMRes, int GEMColour, int WidthRequest, int HeightRequest)
{
  /* Colour depth */
  switch(GEMColour)
  {
    case GEMCOLOUR_2:
      VDIRes = 2;
      VDIPlanes = 1;
      VDIColours = 2;
      VDICharHeight = 16;
      break;
    case GEMCOLOUR_4:
      VDIRes = 1;
      VDIPlanes = 2;
      VDIColours = 4;
      VDICharHeight = 8;
      break;
    case GEMCOLOUR_16:
      VDIRes = 0;
      VDIPlanes = 4;
      VDIColours = 16;
      VDICharHeight = 8;
      break;
  }

  /* Resolution */
  switch(GEMRes)
  {
    case GEMRES_640x480:
      WidthRequest = 640;
      HeightRequest = 480;
      break;
    case GEMRES_800x600:
      WidthRequest = 800;
      HeightRequest = 600;
      break;
    case GEMRES_1024x768:
      WidthRequest = 1024;
      HeightRequest = 768;
      break;
    case GEMRES_OTHER:
      break;
  }
  /* width needs to be aligned to 16 bytes */
  VDIWidth = limit(WidthRequest, 128/VDIPlanes, MIN_VDI_WIDTH, MAX_VDI_WIDTH);
  /* height needs to be multiple of cell height */
  VDIHeight = limit(HeightRequest, VDICharHeight, MIN_VDI_HEIGHT, MAX_VDI_HEIGHT);
  printf("VDI screen: request = %dx%d@%d, aligned result = %dx%d\n",
         WidthRequest, HeightRequest, VDIPlanes, VDIWidth, VDIHeight);

  /* Write resolution to re-boot takes effect with correct bit-depth */
  VDI_FixDesktopInf();
}


/*-----------------------------------------------------------------------*/
/**
 * Check VDI call and see if we need to re-direct to our own routines
 * Return TRUE if we've handled the exception, else return FALSE
 *
 * We enter here with Trap #2, so D1 is pointer to VDI vectors, ie Control, Intin, Ptsin etc...
 */
BOOL VDI(void)
{
  Uint32 TablePtr = Regs[REG_D1];
  /*unsigned short int OpCode;*/

  /* Read off table pointers */
  Control = STMemory_ReadLong(TablePtr);
  Intin = STMemory_ReadLong(TablePtr+4);
  Ptsin = STMemory_ReadLong(TablePtr+8);
  Intout = STMemory_ReadLong(TablePtr+12);
  Ptsout = STMemory_ReadLong(TablePtr+16);

/*
  OpCode = STMemory_ReadWord(Control);
  // Check OpCode
  // 8 - Text Font
  if (OpCode==9)
  {
    return(TRUE);
  }
*/

  /* Call as normal! */
  return FALSE;
}


/*-----------------------------------------------------------------------*/
/**
 * Modify Line-A structure for our VDI resolutions
 */
void VDI_LineA(Uint32 linea, Uint32 fontbase)
{
  if (bUseVDIRes)
  {
    STMemory_WriteWord(linea-46,VDICharHeight);                /* v_cel_ht */
    STMemory_WriteWord(linea-44,(VDIWidth/8)-1);               /* v_cel_mx (cols-1) */
    STMemory_WriteWord(linea-42,(VDIHeight/VDICharHeight)-1);  /* v_cel_my (rows-1) */
    STMemory_WriteWord(linea-40,VDICharHeight*((VDIWidth*VDIPlanes)/8));  /* v_cel_wr */

    STMemory_WriteWord(linea-12,VDIWidth);                      /* v_rez_hz */
    STMemory_WriteWord(linea-4, VDIHeight);                     /* v_rez_vt */
    STMemory_WriteWord(linea-2, (VDIWidth*VDIPlanes)/8);        /* bytes_lin */
    STMemory_WriteWord(linea+0, VDIPlanes);                     /* planes */
    STMemory_WriteWord(linea+2, (VDIWidth*VDIPlanes)/8);        /* width */
  }
  LineABase = linea;
  FontBase = fontbase;
}


/*-----------------------------------------------------------------------*/
/**
 * This is called on completion of a VDI Trap, used to modify return structure for 
 */
void VDI_Complete(void)
{
  unsigned short int OpCode;

  OpCode = STMemory_ReadWord(Control);
  /* Is 'Open Workstation', or 'Open Virtual Screen Workstation'? */
  if ( (OpCode==1) || (OpCode==100) )
  {
    STMemory_WriteWord(Intout,VDIWidth-1);                         /* IntOut[0] Width-1 */
    STMemory_WriteWord(Intout+1*2,VDIHeight-1);                    /* IntOut[1] Height-1 */
    STMemory_WriteWord(Intout+13*2,VDIColours);                    /* IntOut[13] #colours */
    STMemory_WriteWord(Intout+39*2,512);                           /* IntOut[39] #available colours */

    STMemory_WriteWord(LineABase-0x15a*2,VDIWidth-1);              /* WKXRez */
    STMemory_WriteWord(LineABase-0x159*2,VDIHeight-1);             /* WKYRez */

    VDI_LineA(LineABase, FontBase);  /* And modify Line-A structure accordingly */
  }
}


/*-----------------------------------------------------------------------*/
/**
 * Save desktop configuration file for VDI, eg desktop.inf(TOS 1.04) or newdesk.inf(TOS 2.06)
 */
static void VDI_SaveDesktopInf(char *pszFileName, const Uint8 *Script, long ScriptSize)
{
  /* Just save file */
  File_Save(pszFileName, Script, ScriptSize, FALSE);
}


/*-----------------------------------------------------------------------*/
/**
 * Modify exisiting ST desktop configuration files to set resolution(keep user settings)
 */
static void VDI_ModifyDesktopInf(char *pszFileName)
{
  long InfSize;
  Uint8 *pInfData;
  int i;

  /* Load our '.inf' file */
  pInfData = (Uint8 *)File_Read(pszFileName,NULL,&InfSize,NULL);
  if (pInfData)
  {
    /* Scan file for '#E' */
    i = 0;
    while(i<(InfSize-8))
    {
      if ( (pInfData[i]=='#') && (pInfData[i+1]=='E') )
      {
        /* Modify resolution */
        pInfData[i+7] = '1'+VDIRes;
        goto done_modify;
      }

      i++;
    }

done_modify:;
    /* And save */
    File_Save(pszFileName, pInfData, InfSize, FALSE);
    /* Free */
    free(pInfData);
  }
}


/*-----------------------------------------------------------------------*/
/**
 * Modify (or create) ST desktop configuration files so VDI boots up in
 * correct color depth
 */
void VDI_FixDesktopInf(void)
{
  char *szDesktopFileName, *szNewDeskFileName;

  if(!GEMDOS_EMU_ON)
  {
    /* Can't modify DESKTOP.INF when not using GEMDOS hard disk emulation */
    return;
  }

  szDesktopFileName = malloc(2 * FILENAME_MAX);
  if (!szDesktopFileName)
  {
    perror("VDI_FixDesktopInf");
    return;
  }
  szNewDeskFileName = szDesktopFileName + FILENAME_MAX;

  /* Create filenames for hard-drive */
  GemDOS_CreateHardDriveFileName(2, "\\DESKTOP.INF", szDesktopFileName);
  GemDOS_CreateHardDriveFileName(2, "\\NEWDESK.INF", szNewDeskFileName);

  /* First, check if files exist(ie modify or replace) */
  if (!File_Exists(szDesktopFileName))
    VDI_SaveDesktopInf(szDesktopFileName,DesktopScript,sizeof(DesktopScript));
  VDI_ModifyDesktopInf(szDesktopFileName);

  if (!File_Exists(szNewDeskFileName))
    VDI_SaveDesktopInf(szNewDeskFileName,NewDeskScript,sizeof(NewDeskScript));
  VDI_ModifyDesktopInf(szNewDeskFileName);

  free(szDesktopFileName);
}
