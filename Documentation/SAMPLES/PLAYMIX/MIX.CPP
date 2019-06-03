/*
 * VOC file object.  For reading VOC files.
 * Copyright (c) 1993-96 Creative Labs, Inc.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * You have a royalty-free right to use, modify, reproduce and
 * distribute the Sample Files (and/or any modified version) in
 * any way you find useful, provided that you agree that
 * Creative has no warranty obligations or liability for any Sample Files.
 *
 * This is an incomplete program, for demonstration purposes only.
 *
 * by D. Kaden 8/4/93
 */

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "mix.h"



VOCfile::VOCfile(void)
/* class constructor */
{
   stereo=repeat=endoffile=IgnoreBlk1Attr=shifton=false;
   TimeConst=SamplesRemaining=0;
   touse=tousecount=0xFFFF;
}



void VOCfile::SetShiftAmount(unsigned int ToUse)
/* Sets the number of samples to use before one is thrown out.  I recommend
 * using a better pitch shifting algorithm.  This one can only shift up one
 * octave, and shifts up in large steps near its upper limit.  To replace
 * this, I would probably modify a Bresenham line drawing algorithm and use
 * it to decide which samples to use and which to discard.
 */
{
   touse=tousecount=ToUse;
}



int VOCfile::open(char *fname)
/* Open a voc file, verify that it is a voc file.  Returns 0 if successful,
 * error number otherwise. */
{
   if ((f=fopen(fname,"rb"))==NULL) {
      return VOCFILEOPENERROR;
   }

   fseek(f,0x16,SEEK_SET);     /* seek to version number */
   fread(&version, 2, 1, f);   /* read it */
   fread(&idcode, 2, 1, f);    /* read VOC file id code */

   /* complement of version + 0x1234 should equal idcode */
   if (~version + 0x1234 != idcode) {
      return VOCNOTVOCFILE;
   }

   ResetVOC();

   return VOCNOERROR;
}



void VOCfile::ResetVOC(void)
/* Resets the voc file to the first block after the header.  Will also
 * cause the sound to start over.
 */
{
   fseek(f,0x14,SEEK_SET); /* seek to location of offset of first data block */
   fread(&DataBlockOffset, 2, 1, f);      /* read offset */
   fseek(f, DataBlockOffset, SEEK_SET);   /* seek to data block */
   endoffile=false;
   SamplesRemaining=0;
}



boolean VOCfile::ProcessBlock(void)
/* Processes the blocks of data in the voc file.  This function is incom-
 * plete, but is good enough for simple voc files.
 */
{
   boolean DataFound=false;
   unsigned long blklen=0;

   fread(&blocktype, 1, 1, f);         /* read block type */

   switch(blocktype) {
      case 0:                          /* terminator */
         endoffile=true;
         break;
      case 1:                          /* data block */
         fread(&blklen, 3, 1, f);      /* length of data block */
         SamplesRemaining = blklen-2;  /* all but first two are samples */
         if (IgnoreBlk1Attr)
            fseek(f,2,SEEK_CUR);       /* skip TC and Pack bytes */
         else {
            fread(&TimeConst, 1, 1, f);
            fread(&pack, 1, 1, f);
         }
         IgnoreBlk1Attr=false;
         DataFound=true;
         break;
      case 2:                          /* voice continuation */
         fread(&SamplesRemaining, 3, 1, f);
         DataFound=true;
         break;

         /*** CASES 3 - 7 ARE NOT YET SUPPORTED ***/
      case 3:                          /* silence period */
      case 4:                          /* marker block */
      case 5:                          /* ASCII text */
      case 6:                          /* Repeat loop */
      case 7:                          /* End repeat loop */
         fread(&blklen, 3, 1, f);      /* length of this block */
         fseek(f,blklen,SEEK_CUR);     /* skip over it */
         break;
      case 8:                          /* extended block */
         fseek(f, 4, SEEK_CUR);        /* Skip to TC high order byte */
         fread(&TimeConst, 1, 1, f);
         fread(&pack, 1,1,f);
         fread(&stereo, 1,1,f);
         IgnoreBlk1Attr=true;
         break;
   }
   return DataFound;
}



boolean VOCfile::eof(void)
/* Returns true if the terminator block of the voc file has been processed. */
{
   boolean DataFound=false;

   if (!endoffile && SamplesRemaining==0)
      do {
         DataFound=ProcessBlock();
      } while (!(DataFound || endoffile));

   return endoffile;
}



unsigned char VOCfile::GetSample(void)
/* Returns one sound sample from the voc file.  If repeat==true, this func-
 * tion will automatically start the file over after it reaches the end.
 * If repeat==false, this function will return 0x80 (zero volts) after it
 * reaches the end of the file.
 */
{
   boolean DataFound=false;

   if (SamplesRemaining) {          /* still samples in data block */
      fread(&sample, 1, 1, f);
      SamplesRemaining--;
      if (shifton) {                /* pitch shifting */
         if (0==(tousecount--)) {   /* if 0, time to toss a sample */
            tousecount=touse;
            return GetSample();
         } else return sample;
      } else return sample;
   } else {                         /* no samples left in data block */
      if (endoffile) {              /* hit end of file */
         return 0x80;
      } else {                      /* not yet eof, see if more data blocks */
         do {
            DataFound=ProcessBlock();
         } while (!(DataFound || endoffile));

         if (DataFound)
            return GetSample();     /* call myself to get sample */
         else if (endoffile && repeat) {
            ResetVOC();
            return GetSample();
         } else return 0x80;
      }
   }
}


