/** @file

    Copyright (c) 2018, Rafael Rodrigues Machado. All rights reserved.<BR>
    This program and the accompanying materials
    are licensed and made available under the terms and conditions of the BSD License
    which accompanies this distribution. The full text of the license may be found at
    http://opensource.org/licenses/bsd-license.

    THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
    WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

#include<Uefi.h>
#include <Library/UefiLib.h>
#include <IndustryStandard/Pci.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/PciIo.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

#include "HdaLib.h"
#include "PciConfigurations.h"

EFI_PCI_IO_PROTOCOL* HdaPciIoProtocol = NULL;
EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL* RootBridgePciIoProtocol = NULL;


EFI_STATUS TearDownCommonBuffer (
  UINT8 *HostAddress,
  UINTN Length,
  VOID *Mapping)
{
  EFI_STATUS Status;

  Status = HdaPciIoProtocol->Unmap (HdaPciIoProtocol, Mapping);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = HdaPciIoProtocol->FreeBuffer (HdaPciIoProtocol,
                            EFI_SIZE_TO_PAGES (Length),
                            (VOID *)HostAddress);

  return Status;
}


//TODO: Free resources
EFI_STATUS ReleaseHdaPciIoHandler () {

	EFI_STATUS Status = EFI_SUCCESS;

	/*
	Status = TearDownCommonBuffer((UINT8*) CorbAddress,
				   1024, CorbMapping);

	if(!EFI_ERROR(Status)) {
		Status = TearDownCommonBuffer((UINT8*) RirbAddress,
						   	   	   	   2048, RirbMapping);
	}
	*/

	return Status;

}

//Code developed based on the reference code present at
//the UEFI Driver Writer's Guide, page 387
/*
EFI_STATUS
SetupCommonBuffer (
	UINT8 **HostAddress,
	UINTN Length,
	VOID **Mapping)
{
  EFI_STATUS Status;
  UINTN NumberOfBytes;
  EFI_PHYSICAL_ADDRESS DeviceAddress;

  //
  // Allocate a common buffer from anywhere in system memory of
  // type EfiBootServicesData.
  //
  Status = HdaPciIoProtocol->AllocateBuffer (HdaPciIoProtocol, // This
                                AllocateAnyPages, // Type
                                EfiBootServicesData, // MemoryType
                                EFI_SIZE_TO_PAGES (Length), // Pages
                                (VOID **)HostAddress, // HostAddress
                                0// Attributes
                                );

  AllocateAlignedBuffer();

  if (EFI_ERROR (Status)) {
    return Status;
  }

  ZeroMem(*HostAddress, Length);

  //
  // Call Map() to retrieve the DeviceAddress to use for the bus
  // master common buffer operation. If the Map() function cannot
  // support a DMA operation for the entire length, then return an
  // error.
  //
  NumberOfBytes = Length;
  Status = HdaPciIoProtocol->Map (HdaPciIoProtocol, // This
                       EfiPciIoOperationBusMasterCommonBuffer, // Operation
                       (VOID *)*HostAddress, // HostAddress
                       &NumberOfBytes, // NumberOfBytes
                       &DeviceAddress, // DeviceAddress
                       Mapping // Mapping
                       );

  if (!EFI_ERROR (Status) && NumberOfBytes != Length) {

	HdaPciIoProtocol->Unmap (HdaPciIoProtocol, *Mapping);
    Status = ReleaseHdaPciIoHandler();
  }

  if (EFI_ERROR (Status)) {
	  HdaPciIoProtocol->FreeBuffer (HdaPciIoProtocol,
                       EFI_SIZE_TO_PAGES (Length),
                       (VOID *)*HostAddress
                       );
    return Status;
  }

  return Status;
}
*/
EFI_STATUS
SetupCommonBuffer (
	UINT8 **HostAddress,
	UINTN Length,
	VOID **Mapping,
	UINTN Alignment)

{

  *HostAddress = AllocateAlignedPages(
		  	  	  	  EFI_SIZE_TO_PAGES (Length),
					  Alignment);

  return EFI_SUCCESS;
}




EFI_STATUS GetHdaPciIoHandler () {

	UINTN DetectedHandlesCount = 0;
	UINTN CurrentHandle = 0;
	EFI_STATUS Status = EFI_SUCCESS;
	EFI_HANDLE* detectedHandles = NULL;

    UINTN SegmentNumber;
    UINTN BusNumber;
	UINTN DeviceNumber;
	UINTN FunctionNumber;


	if(HdaPciIoProtocol == NULL) {

	  Status = gBS->LocateHandleBuffer(ByProtocol,
	                                 &gEfiPciIoProtocolGuid,
			                         NULL,
			                         &DetectedHandlesCount,
			                         &detectedHandles);

	  if(!EFI_ERROR(Status)) {


		for(CurrentHandle = 0; CurrentHandle < DetectedHandlesCount;
				CurrentHandle++) {

          Status = gBS->OpenProtocol(detectedHandles[CurrentHandle],
								&gEfiPciIoProtocolGuid,
								(VOID**) &HdaPciIoProtocol,
								gImageHandle,
								NULL,
								EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

          HdaPciIoProtocol->GetLocation(HdaPciIoProtocol,
        		                        &SegmentNumber,
										&BusNumber,
										&DeviceNumber,
										&FunctionNumber);

          //This means a HDA controller was detected
          if(BusNumber == HDA_BUS && DeviceNumber == HDA_DEV &&
        		  FunctionNumber == HDA_FUNC) {
        	Print(L"Handle detected\r\n");
            goto FINISH;
          }

          gBS->CloseProtocol(detectedHandles[CurrentHandle],
        					 &gEfiPciIoProtocolGuid,
        	    			 gImageHandle,
        	  				 NULL);

		}
	  }
	}

FINISH:

	return Status;
}


EFI_STATUS GetRootBridgeIoHandler () {

	UINTN handleCount = 0;
    EFI_STATUS Status = EFI_SUCCESS;
	EFI_HANDLE* detectedHandles = NULL;


	if(RootBridgePciIoProtocol == NULL) {

		Status = gBS->LocateHandleBuffer(ByProtocol,
										&gEfiPciRootBridgeIoProtocolGuid,
										 NULL,
										 &handleCount,
										 &detectedHandles);

		if(!EFI_ERROR(Status)) {

		  Status = gBS->OpenProtocol(detectedHandles[0],
									&gEfiPciRootBridgeIoProtocolGuid,
									(VOID**) &RootBridgePciIoProtocol,
									gImageHandle,
									NULL,
									EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
		}

	}

	return Status;
}


EFI_STATUS GetControllerRegisterSet (PCI_HDA_REGION* PcieDeviceConfigSpace,
		                  HDA_CONTROLLER_REGISTER_SET* ControllerRegisterSet) {

	EFI_STATUS Status = EFI_SUCCESS;

	UINTN IssCount = 0;
	UINTN OssCount = 0;
	UINTN BssCount = 0;

	//fix base addres
	UINT64 HdaControllerBar = (PcieDeviceConfigSpace->HDBARL & 0xFFFFFFF0 );


	//Read main registers
	Status = RootBridgePciIoProtocol->Mem.Read(RootBridgePciIoProtocol,
											EfiPciWidthUint8,
											HdaControllerBar,
											sizeof(HDA_CONTROLLER_REGISTER_SET) /
											  sizeof(UINT8),
											(VOID*) ControllerRegisterSet);

	//Now it's time to get the Stream descriptors
	IssCount = HDA_ISS_COUNT(ControllerRegisterSet->GCAP);
	OssCount = HDA_OSS_COUNT(ControllerRegisterSet->GCAP);
	BssCount = HDA_BSS_COUNT(ControllerRegisterSet->GCAP);


	if(IssCount > 0) {

		  ControllerRegisterSet->ISS =
				  AllocateZeroPool(IssCount * sizeof(HDA_CONTROLLER_STREAM_DESCRIPTOR));

		  //Read ISS Descriptors
		  Status = RootBridgePciIoProtocol->Mem.Read(RootBridgePciIoProtocol,
										   EfiPciWidthUint8,
										   HdaControllerBar + CALCULATE_ISSN_OFFSET(0),
										   (sizeof(HDA_CONTROLLER_STREAM_DESCRIPTOR) * IssCount) /
											 sizeof(UINT8),
										   (VOID*) ControllerRegisterSet->ISS);
	}


	if(OssCount > 0) {

		  ControllerRegisterSet->OSS =
				  AllocateZeroPool(OssCount * sizeof(HDA_CONTROLLER_STREAM_DESCRIPTOR));

		  //Read OSS Descriptors
		  Status = RootBridgePciIoProtocol->Mem.Read(RootBridgePciIoProtocol,
										   EfiPciWidthUint8,
										   HdaControllerBar +
											   CALCULATE_OSSN_OFFSET(0, ControllerRegisterSet->GCAP),
											 (sizeof(HDA_CONTROLLER_STREAM_DESCRIPTOR) * OssCount) /
											 sizeof(UINT8),
										   (VOID*) ControllerRegisterSet->OSS);

	}

	if(OssCount > 0) {

		  ControllerRegisterSet->BSS =
				  AllocateZeroPool(BssCount * sizeof(HDA_CONTROLLER_STREAM_DESCRIPTOR));

		  //Read BSS Descriptors
		  Status = RootBridgePciIoProtocol->Mem.Read(RootBridgePciIoProtocol,
										   EfiPciWidthUint8,
										   HdaControllerBar +
											   CALCULATE_BSSN_OFFSET(0, ControllerRegisterSet->GCAP),
										   sizeof(ControllerRegisterSet->BSS) /
											 sizeof(UINT8),
										   (VOID*) ControllerRegisterSet->BSS);

	}


	return Status;
}


EFI_STATUS WritePciMemory (
	UINT64 Address,
	VOID* Value,
	UINTN Count,
	EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH Width)
{

	EFI_STATUS Status = EFI_SUCCESS;


	//Read main registers
	Status = RootBridgePciIoProtocol->Mem.Write(RootBridgePciIoProtocol,
												Width,
				                                Address,
												Count,
												Value);
	return Status;
}

EFI_STATUS ReadPciMemory (
	UINT64 Address,
	VOID* Value,
	UINTN Count,
	EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH Width)
{

    EFI_STATUS Status = EFI_SUCCESS;


    //Read main registers
    Status = RootBridgePciIoProtocol->Mem.Read(RootBridgePciIoProtocol,
										Width,
										Address,
										Count,
										Value);


	return Status;
}


EFI_STATUS WriteControllerRegister (
	PCI_HDA_REGION* PcieDeviceConfigSpace,
	UINT64 Offset,
	VOID* Value,
	UINTN Count,
	UINT8 BarIndex,
	EFI_PCI_IO_PROTOCOL_WIDTH Width)
{

	EFI_STATUS Status = EFI_SUCCESS;
	UINTN VariableWidth = 0;

	switch (Width){
		case EfiPciWidthUint32:
			VariableWidth = sizeof(UINT32);
			break;
		case EfiPciWidthUint16:
			VariableWidth = sizeof(UINT16);
			break;
		case EfiPciWidthUint8:
			VariableWidth = sizeof(UINT8);
			break;
		default:
			Status = EFI_INVALID_PARAMETER;
			goto FINISH;
	}

	VOID* ReadValue = AllocateZeroPool(VariableWidth);

	Status = HdaPciIoProtocol->Mem.Write(HdaPciIoProtocol,
										  Width,
										  BarIndex,
										  Offset,
										  Count,
										  Value);

	 //this read is needed to guarantee that the write transaction
	 //was done correctly, as described at tthe UEFi Driver Writer's guide, page
	 //349
	 Status = HdaPciIoProtocol->Mem.Read(HdaPciIoProtocol,
										 Width,
										 BarIndex,
										 Offset,
										 Count,
										 ReadValue);

	 FreePool(ReadValue);

FINISH:

	 return Status;
}




EFI_STATUS ReadControllerRegister (
	PCI_HDA_REGION* PcieDeviceConfigSpace,
	UINT64 Offset,
	VOID* Value,
	UINTN Count,
	EFI_PCI_IO_PROTOCOL_WIDTH Width)
{

	EFI_STATUS Status = EFI_SUCCESS;

    //fix base addres
    UINT64 HdaControllerBar = (PcieDeviceConfigSpace->HDBARL & 0xFFFFFFF0 );

    Status = RootBridgePciIoProtocol->Mem.Read(RootBridgePciIoProtocol,
    											Width,
												HdaControllerBar + Offset,
												Count,
												Value);


	return Status;
}

EFI_STATUS ReadControllerPcieConfiguration (
	PCI_HDA_REGION* PcieDeviceConfigSpace,
	UINT64 Offset,
	VOID* Value,
	UINTN Count,
	EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH Width)
{

	EFI_STATUS Status = EFI_SUCCESS;

	UINT64 Address = (HDA_BUS << 24) | HDA_DEV << 16 | HDA_FUNC << 8 | Offset;

	Status = RootBridgePciIoProtocol->Pci.Read(RootBridgePciIoProtocol,
											 Width,
											 Address,
											 Count,
											 Value);


	return Status;
}


EFI_STATUS WriteControllerPcieConfiguration (
	PCI_HDA_REGION* PcieDeviceConfigSpace,
	UINT64 Offset,
	VOID* Value,
	UINTN Count,
	EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH Width) {

    EFI_STATUS Status = EFI_SUCCESS;

	UINT64 Address = (HDA_BUS << 24) | HDA_DEV << 16 | HDA_FUNC << 8 | Offset;


	Status = RootBridgePciIoProtocol->Pci.Write(RootBridgePciIoProtocol,
												Width,
												Address,
												Count,
												Value);

	return Status;
}

EFI_STATUS GetPcieConfigSpace (UINTN Bus, UINTN Device, UINTN Function,
		PCI_HDA_REGION* PcieDeviceConfigSpace ) {

  EFI_STATUS Status = EFI_SUCCESS;

  //Uses the first instance of the BridgeIo Protocol to read
  //the information of the PCIE device
  RootBridgePciIoProtocol->Pci.Read(RootBridgePciIoProtocol,
							  EfiPciWidthUint8,
							  EFI_PCI_ADDRESS(Bus, Device, Function, 0),
							  sizeof(PCI_HDA_REGION) / sizeof(UINT8),
							  (VOID*) PcieDeviceConfigSpace);

  return Status;
}

