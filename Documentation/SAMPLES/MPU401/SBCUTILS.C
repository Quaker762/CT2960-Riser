/* -------------------------------------------------------------------------- */
/* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY      */
/* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE        */
/* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR      */
/* PURPOSE.                                                                   */
/*                                                                            */
/* You have a royalty-free right to use, modify, reproduce and                */
/* distribute the Sample Files (and/or any modified version) in               */
/* any way you find useful, provided that you agree that                      */
/* Creative has no warranty obligations or liability for any Sample Files.    */
/*----------------------------------------------------------------------------*/


/* ---------------------------------------------------------------------------
  Program:  Sound Blaster Utilities
  Filename: SBCUTILS.C
  Author:   Scott E. Sindorf
  Language: Borland C
  Date:     30 Jun 93
  
  Copyright (c) 1993 Creative Labs, Inc.
-----------------------------------------------------------------------------*/
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "sbcutils.h"

BLASTREC blastInfo;

/************************************************************************
 * GETBLASTINFO: Retreives BLASTER environment variable.
 *
 * Input: blastInfo - Structure which receives information.
 *
 * Output: result - ERROR if the variable is not found..
 *                  OK if it is found.
 ************************************************************************/

int GetBlastInfo(BLASTREC *blastInfo)
	{
    char *blastStr;
    char *tempStr;
    int result = OK,
        i;

    blastInfo->baseAddr = 0;                                // clear record
    blastInfo->DSPint = 0;
    blastInfo->DMA8 = 0;
    blastInfo->model = 0;
    blastInfo->DMA16 = 0;
    blastInfo->midiPort = 0;

	blastStr = getenv("BLASTER");

	if(blastStr)
		{
        tempStr = strtok(blastStr, " ");
        switch(toupper(tempStr[0]))
            {
            case 'A':
                blastInfo->baseAddr = strtol(tempStr + 1, NULL, 16);
                break;
            case 'I':
                blastInfo->DSPint = (unsigned char) atoi(tempStr + 1);
                break;
            case 'D':
                blastInfo->DMA8 = (unsigned char) atoi(tempStr + 1);
                break;
            case 'T':
                blastInfo->model = (unsigned char) atoi(tempStr + 1);
                break;
            case 'H':
                blastInfo->DMA16 = (unsigned char) atoi(tempStr + 1);
                break;
            case 'P':
                blastInfo->midiPort = strtol(tempStr + 1, NULL, 16);
                break;
            default:
                break;
            }

        for(i = 0; i < 5; i++)
            {
            tempStr = strtok(NULL, " ");
            switch(toupper(tempStr[0]))
                {
                case 'A':
                    blastInfo->baseAddr = strtol(tempStr + 1, NULL, 16);
                    break;
                case 'I':
                    blastInfo->DSPint = (unsigned char) atoi(tempStr + 1);
                    break;
                case 'D':
                    blastInfo->DMA8 = (unsigned char) atoi(tempStr + 1);
                    break;
                case 'T':
                    blastInfo->model = (unsigned char) atoi(tempStr + 1);
                    break;
                case 'H':
                    blastInfo->DMA16 = (unsigned char) atoi(tempStr + 1);
                    break;
                case 'P':
                    blastInfo->midiPort = strtol(tempStr + 1, NULL, 16);
                    break;
                default:
                    break;
                }
            }
		}
    else
        result = ERROR;

    return(result);
	}

/************************************************************************
 * DSPReset: Resets the DSP on the Sound Blaster card.
 *
 * Input:  none
 *
 * Output: OK (0) or ERROR (-1).
 *
 ************************************************************************/
int DSPReset(void)
{
  int result = ERROR,
      i,
      j;

  outp(blastInfo.baseAddr + DSP_RST, 1);
	for(i = 0; i < 100; i++); 									 // delay more than 3us
  outp(blastInfo.baseAddr + DSP_RST, 0);

  for(i = 0; (i < 32) && (result != DSP_RST_OK); i++)
  {
    for(j = 0; (j < 512) && (inp(blastInfo.baseAddr + DSP_RD_ST) < MSB);
      j++);
    if(j < 512)
      result = inp(blastInfo.baseAddr + DSP_RD);
  }
  if(result == DSP_RST_OK)
    result = OK;

  return(result);
}
