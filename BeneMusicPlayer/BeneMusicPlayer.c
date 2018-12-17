/** @file

    Copyright (c) 2018, Rafael Rodrigues Machado. All rights reserved.<BR>
    This program and the accompanying materials
    are licensed and made available under the terms and conditions of the BSD License
    which accompanies this distribution. The full text of the license may be found at
    http://opensource.org/licenses/bsd-license.

    THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
    WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <IndustryStandard/Pci.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/SimpleTextIn.h>

#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

#include "../HdaLib/HdaLib.h"
#include "../HdaLib/HdaParserLib.h"
#include "../HdaLib/PciConfigurations.h"
#include "BeneMusicPlayer.h"

#include "AudioHeaders/hinosDaBandeira.mp3.raw.h"
#include "AudioHeaders/hinoNacionalBrasileiro.mp3.raw.h"


// As defined at the HDa spec,
//the Data must be aligned on a
//128 bit boundary
UINT8* AllignedDataBufferBdlEntry = NULL;
UINT8* SoundData = NULL;
UINTN SoundDataSize = 0;

EFI_INPUT_KEY Key;

VOID EFIAPI EventCallbackFunction(IN EFI_EVENT Event, IN VOID *Context)
{
	UINT32 DmaLowerAddress;
	UINT32 DmaUpperAddress;

	UINT32* PositionInBuffer;

	UINT64 TempUpper;

	EFI_STATUS Status = EFI_SUCCESS;

	//Registers from the HDA controller
	HDA_CONTROLLER_REGISTER_SET ControllerRegisterSet;
	PCI_HDA_REGION PcieDeviceConfigSpace;


	Status = GetPcieConfigSpace(HDA_BUS,
							  HDA_DEV,
							  HDA_FUNC,
							  &PcieDeviceConfigSpace);

	if(!EFI_ERROR(Status)){
		Status = GetControllerRegisterSet(&PcieDeviceConfigSpace,
										   &ControllerRegisterSet);
	}

	ReadControllerRegister(&PcieDeviceConfigSpace,
							HDA_OFFSET_DPIBLBASE,
							(VOID*) &DmaLowerAddress,
							1,
							EfiPciWidthUint32);

	//Set DMA Position Buffer Lower Address
	ReadControllerRegister(&PcieDeviceConfigSpace,
							HDA_OFFSET_DPIBUBASE,
							(VOID*) &DmaUpperAddress ,
							1,
							EfiPciWidthUint32);

	TempUpper = DmaUpperAddress < 32;

	PositionInBuffer =  (UINT32*) (TempUpper |  DmaLowerAddress);

	Print(L"\r\n********DMA Position In Buffer********\r\n");
	//Print(L"P-SD0: 0x%x\r\n", PositionInBuffer[2*0]); //ISS 0
	//Print(L"P-SD1: 0x%x\r\n", PositionInBuffer[2*1]); //ISS 1
	//Print(L"P-SD2: 0x%x\r\n", PositionInBuffer[2*2]); //ISS 2
	//Print(L"P-SD3: 0x%x\r\n", PositionInBuffer[2*3]); //ISS 3
	Print(L"DMA Position OSS 0: %d\r\n", PositionInBuffer[2*4]); //OSS 0
	//Print(L"P-SD5: 0x%x\r\n", PositionInBuffer[2*5]); //OSS 1
	//Print(L"P-SD6: 0x%x\r\n", PositionInBuffer[2*6]); //OSS 2
	//Print(L"P-SD7: 0x%x\r\n", PositionInBuffer[2*7]); //OSS 3
	Print(L"************************\r\n");

	//ParsePcieDeviceData(&PcieDeviceConfigSpace);
	ParseControllerRegisterSet(&ControllerRegisterSet);

	Print(L"AllignedDataBufferBdlEntry:0x%x\r\n", AllignedDataBufferBdlEntry);


	gST->ConOut->SetCursorPosition(gST->ConOut, 0,0);

}

EFI_STATUS InitHdaControllerCodecAndBuffers(
		PCI_HDA_REGION* PcieDeviceConfigSpace,
        HDA_CONTROLLER_REGISTER_SET* ControllerRegisterSet)
{
	EFI_STATUS Status = EFI_SUCCESS;

	UINT32 WriteValue= 0;
	UINT32 Response = 0;

	//Disabling Legacy Interrupts
	Status = DisablePcieInterrupts(PcieDeviceConfigSpace);

	//Disabling Pcie Snooping
	Status = EnablePcieNoSnoop(PcieDeviceConfigSpace);


	//Assign traffic priority to TC0 (TCSEL -> Traffic Class Select Register)
	WriteValue = 0;
	WriteControllerRegister(PcieDeviceConfigSpace,
							HDA_OFFSET_PCIE_TCSEL,
							(VOID*) &WriteValue,
							1,
							0,
							EfiPciWidthUint8);


	Status = AllocateRIRBBuffer(PcieDeviceConfigSpace);
	Status = AllocateCORBBuffer(PcieDeviceConfigSpace);


	//Turn all nodes on
	SendCommandToAllWidgets8BitPayload(PcieDeviceConfigSpace,
							HDA_VRB_SET_POWER_STATE,
							0x0);

	//Mute output widgets
	GetCodecData8BitPayloadCorbRirb(PcieDeviceConfigSpace, 0x0,
								  0x10, HDA_VRB_SET_AMPLIFIER_GAIN_MUTE,
								  INITIAL_VOLUME,  &Response);

	GetCodecData8BitPayloadCorbRirb(PcieDeviceConfigSpace, 0x0,
								  0x11, HDA_VRB_SET_AMPLIFIER_GAIN_MUTE,
								  INITIAL_VOLUME,  &Response);

	return Status;
}

EFI_STATUS AllocateResourcesBasedOnFile(
		PCI_HDA_REGION* PcieDeviceConfigSpace,
        HDA_CONTROLLER_REGISTER_SET* ControllerRegisterSet,
		UINTN FileSize,
		UINTN DataAddress)
{
	EFI_STATUS Status = EFI_SUCCESS;

	UINT32 WriteValue= 0;
	UINT8 WriteValue8= 0;
	UINT16 WriteValue16= 0;
	UINT32 Response = 0;

	VOID* AllignedDataMapping = NULL;

	//Used to split the sound data so the information can be
	//set at several descriptor entries
	UINTN BdlEntriesRequired = 0;
	UINTN BdlEntriesRequiredCurrentEntry = 0;

    //Here the Buffer Descriptor List and the DMA Position buffers will be
	//allocated and set at the controllers configuration space
	Status = AllocateStreamsPages(PcieDeviceConfigSpace,
									ControllerRegisterSet);


	//Calculating how many entries at BDL are needed
	if(SoundDataSize < 0xFFFFFFFF){
		BdlEntriesRequired = 1;
	} else {
		BdlEntriesRequired = SoundDataSize / 0xFFFFFFFF;

		//in case some extra entry is needed
		if(SoundDataSize % 0xFFFFFFFF > 0) {
			BdlEntriesRequired += 1;
		}
	}

	Print(L"sizeof(SoundData): %x\r\n", SoundDataSize);
	Print(L"BdlEntriesRequired: %d\r\n", BdlEntriesRequired);

	for(BdlEntriesRequiredCurrentEntry = 0;
			BdlEntriesRequiredCurrentEntry < 2;
			BdlEntriesRequiredCurrentEntry++) {

		//Allocate the buffers
		Status = SetupCommonBuffer(&AllignedDataBufferBdlEntry,
								SoundDataSize / BdlEntriesRequired,
								&AllignedDataMapping, 2); //dword aligned

		Print(L"BdlEntryAddress %d: 0x%x\r\n",
				BdlEntriesRequiredCurrentEntry,
				AllignedDataBufferBdlEntry);

		//Copy the sound data to the aligned buffer so it
		//can be set at the BDL
		CopyMem(AllignedDataBufferBdlEntry,
				SoundData,
				SoundDataSize / BdlEntriesRequired);

		AddDestriptorListEntryOss0(PcieDeviceConfigSpace,
									ControllerRegisterSet,
									(UINT64) AllignedDataBufferBdlEntry,
									SoundDataSize / BdlEntriesRequired,
									BdlEntriesRequiredCurrentEntry,
									BdlEntriesRequiredCurrentEntry+1);

	}

	//write the cyclic buffer length (the sum of all BDLE sizes)
	WriteValue = SoundDataSize * 2;
	WriteControllerRegister(PcieDeviceConfigSpace,
								CALCULATE_OSSN_OFFSET(0, ControllerRegisterSet->GCAP) +
								HDA_RELATIVE_OFFSET_SDXCBL,
								(VOID*) &WriteValue,
								1,
								0,
								EfiPciWidthUint32);


	//Setup the streamId on the codec nodes (StreamId: 1, Channelid: 0)
	//Page 160 HDA Spec (don't have support for multiple streams yet)
	WriteValue8 = 0x10;
	GetCodecData8BitPayloadCorbRirb(PcieDeviceConfigSpace,
									0,
									0x10,
									HDA_VRB_SET_CHANNEL_STREAM_ID,
									WriteValue8,
									&Response);

	//Set stream id
	WriteValue = 0x100000;
	WriteControllerRegister(PcieDeviceConfigSpace,
							CALCULATE_OSSN_OFFSET(0, ControllerRegisterSet->GCAP) +
							HDA_RELATIVE_OFFSET_SDXCTL,
							(VOID*) &WriteValue,
							1,
							0,
							EfiPciWidthUint32);




	//Set FIFOSize
	WriteValue = 4;
	WriteControllerRegister(PcieDeviceConfigSpace,
							CALCULATE_OSSN_OFFSET(0, ControllerRegisterSet->GCAP) +
							HDA_RELATIVE_OFFSET_SDXFIFOS,
							(VOID*) &WriteValue,
							1,
							0,
							EfiPciWidthUint32);

	//Set the stream format at the controller (2 channel, 16 bits, 44.1KHz)
	WriteValue16 = 0x4011;

	//Set stream format at the codec
	GetCodecData16BitPayloadCorbRirb(PcieDeviceConfigSpace, 0x0,
								  0x10, HDA_VRB_SET_STREAM_FORMAT,
								  WriteValue16, &Response);

	//Set stream format at the codec
	GetCodecData16BitPayloadCorbRirb(PcieDeviceConfigSpace, 0x0,
								  0x11, HDA_VRB_SET_STREAM_FORMAT,
								  WriteValue16, &Response);

	WriteControllerRegister(PcieDeviceConfigSpace,
							CALCULATE_OSSN_OFFSET(0, ControllerRegisterSet->GCAP) +
							HDA_RELATIVE_OFFSET_SDXFMT,
							(VOID*) &WriteValue16,
							1,
							0,
							EfiPciWidthUint16);

	return Status;
}



/**
  The user Entry Point for Application. The user code starts with this function
  as the real entry point for the application.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
UefiMain (
  EFI_HANDLE        ImageHandle,
  EFI_SYSTEM_TABLE  *SystemTable
  )
{
	EFI_STATUS Status = EFI_SUCCESS;

	//Registers from the HDA controller
	HDA_CONTROLLER_REGISTER_SET ControllerRegisterSet;
	PCI_HDA_REGION PcieDeviceConfigSpace;

	UINT32 WriteValue= 0;

	UINT16 CurrentVolume = INITIAL_VOLUME;

	//show possible audios
	{
		UINTN Aux;
		EFI_INPUT_KEY  Key;

		gST->ConOut->ClearScreen(gST->ConOut);

		Print(L"The following audios can be selected:\r\n");
		Print(L"1 - %s\r\n", AudioFiles__hinoNacionalBrasileiro_mp3_raw);
		Print(L"2 - %s\r\n", AudioFiles__hinosDaBandeira_mp3_raw);
		Print(L"Choose one option:");

		gBS->WaitForEvent(1, &(gST->ConIn->WaitForKey), &Aux);
		gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);

		switch (Key.UnicodeChar) {
		case L'1':
			SoundData = (UINT8*) &SoundDataAudioFiles__hinoNacionalBrasileiro_mp3_raw;
			SoundDataSize = AudioFiles__hinoNacionalBrasileiro_mp3_raw_Size;
			break;
		case L'2':
			SoundData = (UINT8*) &SoundDataAudioFiles__hinosDaBandeira_mp3_raw;
			SoundDataSize = AudioFiles__hinosDaBandeira_mp3_raw_Size;
			break;
		default:
			Print(L"\r\nInvalid option. Exiting\r\n");
			goto FINISH;
		}
	}

	Status = InitHdaLib();

	if(!EFI_ERROR(Status)){

		Status = GetPcieConfigSpace(HDA_BUS,
								  HDA_DEV,
								  HDA_FUNC,
								  &PcieDeviceConfigSpace);

		if(!EFI_ERROR(Status)){

			Status = GetControllerRegisterSet(&PcieDeviceConfigSpace,
											   &ControllerRegisterSet);

			InitHdaControllerCodecAndBuffers(&PcieDeviceConfigSpace,
											&ControllerRegisterSet);

			AllocateResourcesBasedOnFile(&PcieDeviceConfigSpace,
										&ControllerRegisterSet, 0,0);

			//set the stream id and play
			Print(L"Ready to play");

			//Stream run.
			WriteValue = 0x100002;

			WriteControllerRegister(&PcieDeviceConfigSpace,
									CALCULATE_OSSN_OFFSET(0, ControllerRegisterSet.GCAP) +
									HDA_RELATIVE_OFFSET_SDXCTL,
									(VOID*) &WriteValue,
									1,
									0,
									EfiPciWidthUint32);

			Print(L"Play done!!!\r\n");

		}

	} else {
		  Print(L"failure to init HdaLib\r\n");
	}


	//create event to monitor the stream processing execution
	{
		UINTN Aux;
		EFI_EVENT   TimerEvent;
		EFI_INPUT_KEY  Key;

		UINT32 Response = 0;

		gST->ConOut->ClearScreen(gST->ConOut);

		Status = gBS->CreateEvent(EVT_TIMER | EVT_NOTIFY_SIGNAL,
								TPL_CALLBACK,
								EventCallbackFunction,
								NULL,
								&TimerEvent);

		gBS->SetTimer(TimerEvent,
						TimerPeriodic,
						10000000);

		do {

			gBS->WaitForEvent(1, &(gST->ConIn->WaitForKey), &Aux);
			gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);

			switch (Key.ScanCode){
				case 0x1: //press page up to increase the volume
					//TODO: Make step size dynamic, based on output step size capabilities
					CurrentVolume += 3;
					GetCodecData8BitPayloadCorbRirb(&PcieDeviceConfigSpace, 0x0,
													  0x10, HDA_VRB_SET_AMPLIFIER_GAIN_MUTE,
													  CurrentVolume,  &Response);
					break;
				case 0x2:  //press page down to decrease the volume
					CurrentVolume -= 3;
					GetCodecData8BitPayloadCorbRirb(&PcieDeviceConfigSpace, 0x0,
													  0x10, HDA_VRB_SET_AMPLIFIER_GAIN_MUTE,
													  CurrentVolume,  &Response);
					break;
				case SCAN_ESC: //press esc to exit
					Print(L"Esc\r\n");

					gRT->ResetSystem(EfiResetWarm, EFI_SUCCESS, 0, NULL);

					break;
				default:
					Print(L"Code:%x", Key.ScanCode);
					break;
			}

		} while (Key.ScanCode != SCAN_ESC);

		gBS->CloseEvent(TimerEvent);

	}

FINISH:

	return Status;
}
