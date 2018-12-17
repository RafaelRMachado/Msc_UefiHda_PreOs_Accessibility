/** @file

    Copyright (c) 2018, Rafael Rodrigues Machado. All rights reserved.<BR>
    This program and the accompanying materials
    are licensed and made available under the terms and conditions of the BSD License
    which accompanies this distribution. The full text of the license may be found at
    http://opensource.org/licenses/bsd-license.

    THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
    WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

#ifndef _BENE_MUSIC_PLAYER_H
#define _BENE_MUSIC_PLAYER_H

/** @file
  This sample application plays some music that is
  programmed at the code.
**/

#include <Uefi.h>

#define STALL_PERIOD_SECOND(SECONDS) \
	((EFI_TIMER_PERIOD_SECONDS(SECONDS) * 100) / 1000)

#define STALL_PERIOD_MICROSECOND(MICROSECONDS) \
	((EFI_TIMER_PERIOD_MICROSECONDS(MICROSECONDS) * 100))

#define BREVE (STALL_PERIOD_SECOND(1) * 4)
#define MINIMA (BREVE/2)
#define SEMINIMA (MINIMA / 2)
#define COLCHEIA (SEMINIMA / 2)
#define SEMICOLCHEIA (COLCHEIA / 2)

#define SILENCIO 0
#define DO (48000/261)
#define RE (48000/293)
#define MI (48000/329)
#define FA (48000/349)
#define SOL (48000/391)
#define LA (48000/440)
#define SI (48000/493)

//Considering 48KHz, each cycle
//takes 1.2 Miliseconds
#define FREQUENCY_PERIOD \
	(STALL_PERIOD_MICROSECOND(1))


#define INITIAL_VOLUME 0x0

#endif
