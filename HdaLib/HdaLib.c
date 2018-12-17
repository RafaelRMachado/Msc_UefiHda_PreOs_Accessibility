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
#include <Protocol/PciIo.h>
#include <Protocol/SimpleFileSystem.h>

#include <Guid/FileInfo.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

#include "PciConfigurations.h"
#include "HdaLib.h"



//Number of rows that are updated by the timer event for debug purposes
#define REFRESH_ROWS 9
#define Bar0 0

STATIC UINT64 CorbCommandCount = 0;

PCI_HDA_REGION* PcieDeviceConfigSpace;
UINT64* DmaPositionBuffer = NULL;
UINT64* AddressBDL = NULL;
INT16* AudioBuffer = NULL;

UINT64 CorbAddress;
VOID* CorbMapping = NULL;
UINT8 CorbLastAddedCommandIndex = 0;

UINT64 RirbAddress;
VOID* RirbMapping = NULL;
UINT8 RirbLastProcessedResponseIndex = 0;



EFI_STATUS InitHdaLib (){

	EFI_STATUS Status = EFI_SUCCESS;

	Status = GetHdaPciIoHandler();

	if(!EFI_ERROR(Status)){
		Status = GetRootBridgeIoHandler();

		if(EFI_ERROR(Status)) {
			Print(L"Fail to detect RootBridgePciIoProtocol!!! \r\n");
		}
	} else {
		Print(L"Fail to detect Hda Controller's PciIoProtocol!!! \r\n");
	}

	return Status;
}

//TODO: Release CORB and RIRB
//Close protocols
//Free Variables
//Close event
EFI_STATUS DisposeHdaLib() {

	return EFI_SUCCESS;

}



EFI_STATUS AddDestriptorListEntryOss0(
		PCI_HDA_REGION* PcieDeviceConfigSpace,
		HDA_CONTROLLER_REGISTER_SET* ControllerRegisterSet,
		UINT64 DataAddress,
		UINT32 DataLength,
		UINT8 BdlEntryIndex,
		UINT32 SdxLastValidIndex)
{

	EFI_STATUS Status = EFI_SUCCESS;
	UINT64 BdlAddress;
	UINT32 BdlLower;
	UINT64 BdlUpper; //the correct size of this variable is UINT32
					//but to make things easier when reconstructing the BDLAddress
					//we'll keep it UINT64


	HDA_BUFFER_DESCRIPTOR_LIST* DescriptorList = NULL;


	Status = ReadControllerRegister(PcieDeviceConfigSpace,
									CALCULATE_OSSN_OFFSET(0, ControllerRegisterSet->GCAP) +
									HDA_RELATIVE_OFFSET_SDXBDPL,
									(VOID*) &BdlLower,
									1,
									EfiPciWidthUint32);

	Status = ReadControllerRegister(PcieDeviceConfigSpace,
									CALCULATE_OSSN_OFFSET(0, ControllerRegisterSet->GCAP) +
									HDA_RELATIVE_OFFSET_SDXBDPU,
									(VOID*) &BdlUpper,
									1,
									EfiPciWidthUint32);

	BdlAddress = BdlLower | (BdlUpper << 32);

	Print(L"BdlLower: 0x%x\r\n", BdlLower);
	Print(L"BdlUpper: 0x%x\r\n", BdlUpper);
	Print(L"BdlAddress: 0x%x\r\n", BdlAddress);

	Print(L"DataLength OSSSet: 0x%x\r\n", DataLength);

	//Since there must be at least 2 entries at the
	//bdl, we'll put half of the buffer on each entry
	DescriptorList = (HDA_BUFFER_DESCRIPTOR_LIST*) BdlAddress;

	DescriptorList->BDLEntry[BdlEntryIndex].Address = DataAddress;
	DescriptorList->BDLEntry[BdlEntryIndex].Length = DataLength;

	//TODO: Enable this after Corb/Rirb start to process unsolicited responses
	DescriptorList->BDLEntry[BdlEntryIndex].IntrptOnComp = 1;

	Print(L"DescriptorList->BDLEntry[%d].Address: 0x%x\r\n",BdlEntryIndex, DescriptorList->BDLEntry[BdlEntryIndex].Address);
	Print(L"DescriptorList->BDLEntry[%d].Length: 0x%x\r\n", BdlEntryIndex, DescriptorList->BDLEntry[BdlEntryIndex].Length);
	Print(L"DescriptorList->BDLEntry[%d].IntrptOnComp: 0x%x\r\n", BdlEntryIndex, DescriptorList->BDLEntry[BdlEntryIndex].IntrptOnComp);


	//Set the number of valid buffers at the DBL, as defined at page 47 of the HDA Spec
	Status = WriteControllerRegister(PcieDeviceConfigSpace,
									CALCULATE_OSSN_OFFSET(0, ControllerRegisterSet->GCAP) +
										HDA_RELATIVE_OFFSET_SDXLVI,
									(VOID*) &SdxLastValidIndex, //since we have two BDLE we're setting this to 2
									1,
									Bar0,
									EfiPciWidthUint32);

	return Status;
}

EFI_STATUS AllocateStreamsPages(
		PCI_HDA_REGION* PcieDeviceConfigSpace,
		HDA_CONTROLLER_REGISTER_SET* ControllerRegisterSet)
{
	EFI_STATUS Status = EFI_SUCCESS;

	UINT8* BdlAddressPointer = NULL;
	VOID* BdlMapping = NULL;
	UINT64 BdlAddress;

	//These variables are related to the DMAPositionBuffer
	UINT8* DmaAddressPointer = NULL;
	VOID* DmaMapping = NULL;
	UINT64 DmaAddress;

	UINT32 WriteValue = 0;

	UINTN IssCount = (ControllerRegisterSet->GCAP >> 8) & 0xF;
	UINTN OssCount = (ControllerRegisterSet->GCAP >> 12) & 0xF;
	UINTN BssCount = (ControllerRegisterSet->GCAP >> 3) & 0x1F;
	UINTN Count = 0;

	for(Count = 0; Count < IssCount; Count++) {

		//TODO: Free this buffer
		Status = SetupCommonBuffer(&BdlAddressPointer,
								  sizeof(HDA_BUFFER_DESCRIPTOR_LIST),
								 &BdlMapping, 16); //128 bit aligned

		BdlAddress = (UINT64) BdlAddressPointer;

		//Set BDL Lower Address
		WriteValue = (BdlAddress & 0xFFFFFFFF);
		WriteControllerRegister(PcieDeviceConfigSpace,
								CALCULATE_ISSN_OFFSET(Count) +
								HDA_RELATIVE_OFFSET_SDXBDPL,
								(VOID*) &WriteValue,
								1,
								Bar0,
								EfiPciWidthUint32);

		//Set BDL Upper Address
		WriteValue = (BdlAddress >> 32);
		WriteControllerRegister(PcieDeviceConfigSpace,
								CALCULATE_ISSN_OFFSET(Count) +
								HDA_RELATIVE_OFFSET_SDXBDPU,
								(VOID*) &WriteValue ,
								1,
								Bar0,
								EfiPciWidthUint32);

		Print(L"BDLAddress: 0x%x\r\n", BdlAddress);
	}



	for(Count = 0; Count < OssCount; Count++) {
		//TODO: Free this buffer
		Status = SetupCommonBuffer(&BdlAddressPointer,
								sizeof(HDA_BUFFER_DESCRIPTOR_LIST),
								 &BdlMapping, 16);

		BdlAddress = (UINT64) BdlAddressPointer;

		//Set Corb Lower Address
		WriteValue = (BdlAddress & 0xFFFFFFFF);
		WriteControllerRegister(PcieDeviceConfigSpace,
								CALCULATE_OSSN_OFFSET(Count, ControllerRegisterSet->GCAP) +
								HDA_RELATIVE_OFFSET_SDXBDPL,
								(VOID*) &WriteValue,
								1,
								Bar0,
								EfiPciWidthUint32);

		//Set Corb Lower Address
		WriteValue = (BdlAddress >> 32);
		WriteControllerRegister(PcieDeviceConfigSpace,
								CALCULATE_OSSN_OFFSET(Count, ControllerRegisterSet->GCAP) +
								HDA_RELATIVE_OFFSET_SDXBDPU,
								(VOID*) &WriteValue ,
								1,
								Bar0,
								EfiPciWidthUint32);
	}

	for(Count = 0; Count < BssCount; Count++) {
		//TODO: Free this buffer
		Status = SetupCommonBuffer(&BdlAddressPointer,
								sizeof(HDA_BUFFER_DESCRIPTOR_LIST),
								 &BdlMapping, 16);

		BdlAddress = (UINT64) BdlAddressPointer;

		//Set Corb Lower Address
		WriteValue = (BdlAddress & 0xFFFFFFFF);
		WriteControllerRegister(PcieDeviceConfigSpace,
								CALCULATE_BSSN_OFFSET(Count, ControllerRegisterSet->GCAP) +
								HDA_RELATIVE_OFFSET_SDXBDPL,
								(VOID*) &WriteValue,
								1,
								Bar0,
								EfiPciWidthUint32);

		//Set Corb Lower Address
		WriteValue = (BdlAddress >> 32);
		WriteControllerRegister(PcieDeviceConfigSpace,
								CALCULATE_BSSN_OFFSET(Count, ControllerRegisterSet->GCAP) +
								HDA_RELATIVE_OFFSET_SDXBDPU,
								(VOID*) &WriteValue ,
								1,
								Bar0,
								EfiPciWidthUint32);
	}


	//Allocate the 	DMAPosition Buffer
	//This buffer's size is explained at page 55 of thd HDA spec
	//TODO: Free this buffer
	Status = SetupCommonBuffer(&DmaAddressPointer,
							  (IssCount + OssCount + BssCount )
							  	  * sizeof(UINT64),
							 &DmaMapping, 16);

	DmaAddress = (UINT64) DmaAddressPointer;

	//Set DMA Position Buffer Lower Address
	WriteValue = (DmaAddress & 0xFFFFFFFF);
	WriteValue |= 0x1; //The fisrt bit is related to enabling the DMAPosition buffer
	WriteControllerRegister(PcieDeviceConfigSpace,
							HDA_OFFSET_DPIBLBASE,
							(VOID*) &WriteValue,
							1,
							Bar0,
							EfiPciWidthUint32);

	//Set DMA Position Buffer Lower Address
	WriteValue = (DmaAddress >> 32);
	WriteControllerRegister(PcieDeviceConfigSpace,
							HDA_OFFSET_DPIBUBASE,
							(VOID*) &WriteValue ,
							1,
							Bar0,
							EfiPciWidthUint32);



	return Status;
}



EFI_STATUS AllocateCORBBuffer(PCI_HDA_REGION* PcieDeviceConfigSpace)
{

  EFI_STATUS Status = EFI_SUCCESS;
  UINT8* CorbAddressPointer = NULL;
  UINT32 ReadValue32 = 0;
  UINT32 WriteValue = 0;
  UINT8 WriteValue8 = 0;


  //Sets the corb address to a DMA allocated Buffer
  //TODO: Free this buffer
  Status = SetupCommonBuffer(&CorbAddressPointer, 1024,
							 &CorbMapping, 16);

  CorbAddress = (UINT64) CorbAddressPointer;

  //Set Corb Lower Address
  WriteValue = (CorbAddress & 0xFFFFFFFF);
  WriteControllerRegister(PcieDeviceConfigSpace,
							HDA_OFFSET_CORBLBASE,
							(VOID*) &WriteValue,
							1,
							Bar0,
							EfiPciWidthUint32);

  //Set Corb Lower Address
  WriteValue = (CorbAddress >> 32);
  WriteControllerRegister(PcieDeviceConfigSpace,
							HDA_OFFSET_CORBUBASE,
							(VOID*) &WriteValue ,
							1,
							Bar0,
							EfiPciWidthUint32);


  //set write pointer to 0
  WriteValue = 0;
  WriteControllerRegister(PcieDeviceConfigSpace,
							HDA_OFFSET_CORBWP,
							(VOID*) &WriteValue,
							1,
							Bar0,
							EfiPciWidthUint32);

  //Reset read pointer
  WriteValue = 0x8000;
  WriteControllerRegister(PcieDeviceConfigSpace,
							HDA_OFFSET_CORBRP,
							(VOID*) &WriteValue,
							1,
							Bar0,
							EfiPciWidthUint32);

  ReadControllerRegister(PcieDeviceConfigSpace,
						   HDA_OFFSET_CORBRP,
						   (VOID*) &ReadValue32,
						   1,
						   EfiPciWidthUint32);

  WriteValue = 0;
  WriteControllerRegister(PcieDeviceConfigSpace,
							HDA_OFFSET_CORBRP,
							(VOID*) &WriteValue ,
							1,
							Bar0,
							EfiPciWidthUint32);

  //start verb processing via CORB. And also enable the CMEIE
  WriteValue8 = 0x3;
  WriteControllerRegister(PcieDeviceConfigSpace,
						   HDA_OFFSET_CORBCTL,
						   (VOID*) &WriteValue8,
						   1,
						   Bar0,
						   EfiPciWidthUint8);

  Print(L"CorbAddress: %x\r\n", CorbAddress);
  Print(L"CorbMapping: %x\r\n", CorbMapping);

  return Status;
}


EFI_STATUS AllocateRIRBBuffer(PCI_HDA_REGION* PcieDeviceConfigSpace)
{
	EFI_STATUS Status = EFI_SUCCESS;
	UINT8* RirbAddressPointer = NULL;

	UINT32 WriteValue = 0;
	UINT8 WriteValue8 = 0;


	//Sets the corb address to a DMA allocated Buffer
	//TODO: Free this buffer
	Status = SetupCommonBuffer(&RirbAddressPointer, 2048,
							 &RirbMapping, 16);

	RirbAddress = (UINT64) RirbAddressPointer;

	//Set Lower Address
	WriteValue = (RirbAddress & 0xFFFFFFFF);
	WriteControllerRegister(PcieDeviceConfigSpace,
							HDA_OFFSET_RIRBLBASE,
							(VOID*) &WriteValue,
							1,
							Bar0,
							EfiPciWidthUint32);

	//Set Corb Lower Address
	WriteValue = (RirbAddress >> 32);
	WriteControllerRegister(PcieDeviceConfigSpace,
							HDA_OFFSET_RIRBUBASE,
							(VOID*) &WriteValue,
							1,
							Bar0,
							EfiPciWidthUint32);

	//set WP and RP to offset 0 off the Corb Address
	WriteValue = 0;
	WriteControllerRegister(PcieDeviceConfigSpace,
							HDA_OFFSET_RIRBWP,
							(VOID*) &WriteValue,
							1,
							Bar0,
							EfiPciWidthUint32);

	//start RIRB engine to receive the answers
	WriteValue8 = 0x2;
	WriteControllerRegister(PcieDeviceConfigSpace,
							HDA_OFFSET_RIRBCTL,
							(VOID*) &WriteValue8,
							1,
							Bar0,
							EfiPciWidthUint8);

	Print(L"RirbAddress: %x\r\n", RirbAddress);
	Print(L"RirbMapping: %x\r\n", RirbMapping);



	return Status;
}


EFI_STATUS FillCodecNode(PCI_HDA_REGION* PcieDeviceConfigSpace,
		                  UINT32 CurrentNodeId,
		                  HDA_NODE_TYPE NodeType,
		                  struct Node *CurrentNode) {

	UINT32 CurrentSubordinateNodeCount = 0;

	struct Node TempNode;

	TempNode.NodeId = CurrentNodeId;
	TempNode.NodeType = NodeType;

	//Retrieving node (root / function group/ widget) data

	GetCodecData8BitPayloadCorbRirb(PcieDeviceConfigSpace,0,CurrentNodeId,HDA_VRB_GET_PARAMETER,
			HDA_PARAM_VENDOR_ID, &TempNode.VendorId);

	GetCodecData8BitPayloadCorbRirb(PcieDeviceConfigSpace,0,CurrentNodeId,HDA_VRB_GET_PARAMETER,
			HDA_PARAM_REVISION_ID, &TempNode.RevisionId);

	GetCodecData8BitPayloadCorbRirb(PcieDeviceConfigSpace,0,CurrentNodeId,HDA_VRB_GET_PARAMETER,
			HDA_PARAM_SUBORDINATE_NODE_COUNT, &CurrentSubordinateNodeCount);

	GetCodecData8BitPayloadCorbRirb(PcieDeviceConfigSpace,0,CurrentNodeId,HDA_VRB_GET_PARAMETER,
				HDA_PARAM_FUNCTION_GROUP_TYPE, &TempNode.FunctionGroupType);

	GetCodecData8BitPayloadCorbRirb(PcieDeviceConfigSpace,0,CurrentNodeId,HDA_VRB_GET_PARAMETER,
			HDA_PARAM_AUDIO_FUNC_CAP, &TempNode.FuncCap);

	GetCodecData8BitPayloadCorbRirb(PcieDeviceConfigSpace,0,CurrentNodeId,HDA_VRB_GET_PARAMETER,
    		HDA_PARAM_AUDIO_WIDGET_CAP, &TempNode.WidgetCap);

	GetCodecData8BitPayloadCorbRirb(PcieDeviceConfigSpace,0,CurrentNodeId,HDA_VRB_GET_PARAMETER,
    		HDA_PARAM_SAMPLE_SIZE_RATE_CAP, &TempNode.SampleSizeRateCap);

	GetCodecData8BitPayloadCorbRirb(PcieDeviceConfigSpace,0,CurrentNodeId,HDA_VRB_GET_PARAMETER,
    		HDA_PARAM_STREAM_FORMATS, &TempNode.StreamFormat);

	GetCodecData8BitPayloadCorbRirb(PcieDeviceConfigSpace,0,CurrentNodeId,HDA_VRB_GET_PARAMETER,
    		HDA_PARAM_PIN_CAP, &TempNode.PinCap);

	GetCodecData8BitPayloadCorbRirb(PcieDeviceConfigSpace,0,CurrentNodeId,HDA_VRB_GET_PARAMETER,
    		HDA_PARAM_INPUT_AMP_CAP, &TempNode.InputAmpCap);

	GetCodecData8BitPayloadCorbRirb(PcieDeviceConfigSpace,0,CurrentNodeId,HDA_VRB_GET_PARAMETER,
    		HDA_PARAM_OUTPUT_AMP_CAP, &TempNode.OutputAmpCap);

	GetCodecData8BitPayloadCorbRirb(PcieDeviceConfigSpace,0,CurrentNodeId,HDA_VRB_GET_PARAMETER,
    		HDA_PARAM_CONNECTION_LIST_LENGTH, &TempNode.ConnectionListLength);

	GetCodecData8BitPayloadCorbRirb(PcieDeviceConfigSpace,0,CurrentNodeId,HDA_VRB_GET_PARAMETER,
    		HDA_PARAM_SUPPORTED_POWER_STATES, &TempNode.SupportedPowerStates);

	GetCodecData8BitPayloadCorbRirb(PcieDeviceConfigSpace,0,CurrentNodeId,HDA_VRB_GET_PARAMETER,
    		HDA_PARAM_PROCESSING_CAP, &TempNode.ProcessingCap);

	GetCodecData8BitPayloadCorbRirb(PcieDeviceConfigSpace,0,CurrentNodeId,HDA_VRB_GET_PARAMETER,
    		HDA_PARAM_GPIO_COUNT, &TempNode.GPIOCount);

	GetCodecData8BitPayloadCorbRirb(PcieDeviceConfigSpace,0,CurrentNodeId,HDA_VRB_GET_PARAMETER,
    		HDA_PARAM_VOLUME_KNOB_CAP, &TempNode.VolKnobCap);

	GetCodecData8BitPayloadCorbRirb(PcieDeviceConfigSpace,0,CurrentNodeId,HDA_VRB_GET_POWER_STATE, 0,
				                &TempNode.PowerState);

	GetCodecData8BitPayloadCorbRirb(PcieDeviceConfigSpace,0,CurrentNodeId, HDA_VRB_GET_CHANNEL_STREAM_ID, 0,
					            &TempNode.ChannelStreamId);

    //Adjusting information for better usage
	TempNode.StartingChildNodeAddess =
			HDA_SUB_NODE_COUNT_START_NODE(CurrentSubordinateNodeCount);

	TempNode.SubordinateNodeCount =
			HDA_SUB_NODE_COUNT_TOTAL_NODE(CurrentSubordinateNodeCount);

	TempNode.WidgetType = HDA_WIDGET_TYPE(TempNode.WidgetCap);

	TempNode.RightGain = GetAmplifierGain(PcieDeviceConfigSpace,
											TempNode.NodeId,
			                                  TRUE, FALSE);

	TempNode.LeftGain = GetAmplifierGain(PcieDeviceConfigSpace,
											TempNode.NodeId,
											  TRUE, TRUE);

	CopyMem(CurrentNode, &TempNode, sizeof(struct Node));

	return EFI_SUCCESS;
}


EFI_STATUS GetNodeById(struct Node *RootNode,
		               UINT32 NodeIdToSearch,
		               struct Node* NodeDetected) {

	struct Node *CurrentFunctionGroupNode = NULL;
	struct Node *CurrentWidgetNode = NULL;

	UINT32 CurrentFunctionGroupId = 0;
	UINT32 CurrentFgArrayIndexer = 0;

	UINT32 CurrentWidgetId = 0;
	UINT32 CurrentWidgetArrayIndexer = 0;

	for(CurrentFunctionGroupId = RootNode->StartingChildNodeAddess;
		CurrentFunctionGroupId < RootNode->StartingChildNodeAddess +
		RootNode->SubordinateNodeCount; CurrentFunctionGroupId++,
		CurrentFgArrayIndexer++) {

		//Fills function group information
		CurrentFunctionGroupNode = &(RootNode->ChildNodes[CurrentFgArrayIndexer]);

		//In case the node being searched is a function group node
		if(CurrentFunctionGroupNode->NodeId == NodeIdToSearch) {

			CopyMem(NodeDetected, CurrentFunctionGroupNode,
					sizeof(struct Node));

			goto FINISH;
		}

		CurrentWidgetArrayIndexer = 0;

		for(CurrentWidgetId = CurrentFunctionGroupNode->StartingChildNodeAddess;
			CurrentWidgetId < CurrentFunctionGroupNode->StartingChildNodeAddess +
			CurrentFunctionGroupNode->SubordinateNodeCount - 1;
			CurrentWidgetId++, CurrentWidgetArrayIndexer++) {

			CurrentWidgetNode = &(CurrentFunctionGroupNode->ChildNodes[CurrentWidgetArrayIndexer]);

			//In case the node being searched is a widget node
			if(CurrentWidgetNode->NodeId == NodeIdToSearch) {

				CopyMem(NodeDetected, CurrentWidgetNode,
									sizeof(struct Node));

				goto FINISH;
			}

		}
	}

FINISH:

	return EFI_SUCCESS;
}


EFI_STATUS GetCodecTree (PCI_HDA_REGION* PcieDeviceConfigSpace,
		struct Node *RootNode) {

    EFI_STATUS Status = EFI_SUCCESS;
    struct Node *CurrentFunctionGroupNode = NULL;
    struct Node *CurrentWidgetNode = NULL;

    UINT32 CurrentFunctionGroupId = 0;
    UINT32 CurrentFgArrayIndexer = 0;

    UINT32 CurrentWidgetId = 0;
    UINT32 CurrentWidgetArrayIndexer = 0;


    //fills the root node information
    FillCodecNode(PcieDeviceConfigSpace, 0, HDA_NODE_ROOT, RootNode);

    //Allocate space for the function groups
    RootNode->ChildNodes = AllocateZeroPool(sizeof(struct Node) *
    		                  	  	  	  (RootNode->SubordinateNodeCount -
    		                  	  	  	  RootNode->StartingChildNodeAddess));

    for(CurrentFunctionGroupId = RootNode->StartingChildNodeAddess;
    		CurrentFunctionGroupId < RootNode->StartingChildNodeAddess +
    		RootNode->SubordinateNodeCount; CurrentFunctionGroupId++,
    		CurrentFgArrayIndexer++) {

    	//Fills function group information
    	CurrentFunctionGroupNode = &(RootNode->ChildNodes[CurrentFgArrayIndexer]);
    	FillCodecNode(PcieDeviceConfigSpace, CurrentFunctionGroupId,
    			      HDA_NODE_FUNCTION_GROUP, CurrentFunctionGroupNode);

    	CurrentFunctionGroupNode->ChildNodes = AllocateZeroPool(sizeof(struct Node) *
    			                                   CurrentFunctionGroupNode->StartingChildNodeAddess +
    			                                   CurrentFunctionGroupNode->SubordinateNodeCount);

    	CurrentWidgetArrayIndexer = 0;

    	for(CurrentWidgetId = CurrentFunctionGroupNode->StartingChildNodeAddess;
    			CurrentWidgetId < CurrentFunctionGroupNode->StartingChildNodeAddess +
    			CurrentFunctionGroupNode->SubordinateNodeCount - 1;
    			CurrentWidgetId++, CurrentWidgetArrayIndexer++) {

    		//fills widget information
    		CurrentWidgetNode = &(CurrentFunctionGroupNode->ChildNodes[CurrentWidgetArrayIndexer]);
    		FillCodecNode(PcieDeviceConfigSpace, CurrentWidgetId,
    				      HDA_NODE_WIDGET, CurrentWidgetNode);

    	}


    }

	return Status;
}


EFI_STATUS ReleaseCodecTree (PCI_HDA_REGION* PcieDeviceConfigSpace,
		struct Node *RootNode) {

    EFI_STATUS Status = EFI_SUCCESS;
    struct Node *CurrentFunctionGroupNode = NULL;

    UINT32 CurrentFunctionGroupId = 0;
    UINT32 CurrentFgArrayIndexer = 0;

    for(CurrentFunctionGroupId = RootNode->StartingChildNodeAddess;
    		CurrentFunctionGroupId < RootNode->StartingChildNodeAddess +
    		RootNode->SubordinateNodeCount - 1; CurrentFunctionGroupId++,
    		CurrentFgArrayIndexer++) {

    	CurrentFunctionGroupNode = &(RootNode->ChildNodes[CurrentFgArrayIndexer]);
    	FreePool(CurrentFunctionGroupNode->ChildNodes);

    }

    FreePool(RootNode->ChildNodes);

	return Status;
}





EFI_STATUS AddCommandToCorb(UINT32 VerbToSend) {


	CorbLastAddedCommandIndex += 1;

	//Since CORB is a ring buffer, we need to go back to the
	//initial position after receiving the max number of entries
	if(CorbLastAddedCommandIndex >= 256) {
		CorbLastAddedCommandIndex = 0;
	}


	//Copy the verb to the corb buffer
	//It's important to remember that the first command to be processed must be
	//placed at corb address + 32.
	//This is explained at the HDA Spec at page 65
	WritePciMemory(CorbAddress + (CorbLastAddedCommandIndex * sizeof(UINT32)),
					(VOID*) &VerbToSend,
					1,
					EfiPciWidthUint32);

	//Update the write pointer so the DMA engine understands that
	//there is a new entry to be processed
	WriteControllerRegister(PcieDeviceConfigSpace,
								HDA_OFFSET_CORBWP,
								(VOID*) &CorbLastAddedCommandIndex,
								1,
								Bar0,
								EfiPciWidthUint32);

	CorbCommandCount+=1;
/*
	Print(L"CorbCommandCount = %d\r\n", CorbCommandCount);
	Print(L"VerbToSend = 0x%x\r\n", VerbToSend);
	Print(L"CorbLastAddedCommandIndex = %d\r\n", CorbLastAddedCommandIndex);
*/

	return EFI_SUCCESS;

}


EFI_STATUS ReadResponseFromRirb(UINT64* VerbResponse) {

	EFI_STATUS Status = EFI_SUCCESS;

	RirbLastProcessedResponseIndex += 1;

	if(RirbLastProcessedResponseIndex >= 256)
	{
		RirbLastProcessedResponseIndex = 0;
	}



	//Read the answer from the RIRB Buffer
	//It's important to remember that the first command's response
	//will be placed at rirb address + 64.
	//This is explained at the HDA Spec at page 65
	Status = ReadPciMemory(RirbAddress +
						(RirbLastProcessedResponseIndex *
								sizeof(UINT64)),
						VerbResponse, 1,
						EfiPciWidthUint64);

	return Status;

}

EFI_STATUS GetCodecData8BitPayloadCorbRirb (
  PCI_HDA_REGION* PcieDeviceConfigSpace,
  UINT8 CodecAddress, UINT8 NodeId,
  HDA_VERB Verb, UINT8 VerbPayload,
  UINT32 *Response)
{

    HDA_COMMAND_FIELD_8BIT_PAYLOAD comando;
    HDA_RESPONSE_FIELD* RawResponse;

    UINT32 VerbToSend = 0;
    UINT64 TempResponse = 0;

	comando.CAd = CodecAddress;
	comando.NID = NodeId;
	comando.VerbIdent = Verb;
	comando.VerbPayload = VerbPayload;

	CopyMem(&VerbToSend, &comando, sizeof(UINT32));

	//Send the command
	AddCommandToCorb(VerbToSend);

	//wait some time for the processing
	gBS->Stall(200);

	//Read the answer
	ReadResponseFromRirb(&TempResponse);

	RawResponse = (HDA_RESPONSE_FIELD*) &TempResponse;

	CopyMem(Response, &RawResponse->Response, sizeof(UINT32));

	return EFI_SUCCESS;
}

EFI_STATUS GetCodecData16BitPayloadCorbRirb (
  PCI_HDA_REGION* PcieDeviceConfigSpace,
  UINT8 CodecAddress, UINT8 NodeId,
  HDA_VERB Verb, UINT16 VerbPayload,
  UINT32 *Response)
{

	HDA_COMMAND_FIELD_16BIT_PAYLOAD comando;
    HDA_RESPONSE_FIELD* RawResponse;

    UINT32 VerbToSend = 0;
    UINT64 TempResponse = 0;

	comando.CAd = CodecAddress;
	comando.NID = NodeId;
	comando.VerbIdent = Verb;
	comando.VerbPayload = VerbPayload;

	CopyMem(&VerbToSend, &comando, sizeof(UINT32));

	//Send the command
	AddCommandToCorb(VerbToSend);

	//wait some time for the processing
	gBS->Stall(200);

	//Read the answer
	ReadResponseFromRirb(&TempResponse);

	RawResponse = (HDA_RESPONSE_FIELD*) &TempResponse;

	CopyMem(Response, &RawResponse->Response, sizeof(UINT32));

	return EFI_SUCCESS;
}

EFI_STATUS SendCommandToAllWidgets8BitPayload (
		PCI_HDA_REGION* PcieDeviceConfigSpace,
		HDA_VERB Verb, UINT8 VerbPayload)
{
    EFI_STATUS Status = EFI_SUCCESS;

    struct Node RootNode;
    UINT32 Response;

    GetCodecTree(PcieDeviceConfigSpace, &RootNode);

    struct Node *CurrentFunctionGroupNode = NULL;
    struct Node *CurrentWidgetNode = NULL;

    UINT32 CurrentFunctionGroupId = 0;
    UINT32 CurrentFgArrayIndexer = 0;

    UINT32 CurrentWidgetId = 0;
    UINT32 CurrentWidgetArrayIndexer = 0;

    //Sends the command to the root node
    GetCodecData8BitPayloadCorbRirb(PcieDeviceConfigSpace, 0,
									0, Verb,
									VerbPayload, &Response);
    Print(L"Verb 0x%x with payload 0x%x sent to node 0x0\r\n",
        				Verb, VerbPayload);

    for(CurrentFunctionGroupId = RootNode.StartingChildNodeAddess;
    		CurrentFunctionGroupId < RootNode.StartingChildNodeAddess +
    		RootNode.SubordinateNodeCount; CurrentFunctionGroupId++,
    		CurrentFgArrayIndexer++) {

        //Sends the command to the function group node
        GetCodecData8BitPayloadCorbRirb(PcieDeviceConfigSpace, 0,
        		CurrentFunctionGroupId, Verb,
    									VerbPayload, &Response);
        Print(L"Verb 0x%x with payload 0x%x sent to node 0x%x\r\n",
            				Verb, VerbPayload, CurrentFunctionGroupId);

    	//Fills function group information
    	CurrentFunctionGroupNode = &(RootNode.ChildNodes[CurrentFgArrayIndexer]);

    	CurrentWidgetArrayIndexer = 0;

    	for(CurrentWidgetId = CurrentFunctionGroupNode->StartingChildNodeAddess;
    			CurrentWidgetId < CurrentFunctionGroupNode->StartingChildNodeAddess +
    			CurrentFunctionGroupNode->SubordinateNodeCount - 1;
    			CurrentWidgetId++, CurrentWidgetArrayIndexer++) {

    		//fills widget information
    		CurrentWidgetNode = &(CurrentFunctionGroupNode->ChildNodes[CurrentWidgetArrayIndexer]);

    		GetCodecData8BitPayloadCorbRirb(PcieDeviceConfigSpace, 0,
    										CurrentWidgetId, Verb,
											VerbPayload, &Response);

    		Print(L"Verb 0x%x with payload 0x%x sent to node 0x%x\r\n",
    				Verb, VerbPayload, CurrentWidgetId);
    	}
    }

    ReleaseCodecTree(PcieDeviceConfigSpace, &RootNode);

	return Status;
}

EFI_STATUS SendCommandToAllWidgets16BitPayload (
		PCI_HDA_REGION* PcieDeviceConfigSpace,
		HDA_VERB Verb, UINT16 VerbPayload)
{
    EFI_STATUS Status = EFI_SUCCESS;

    struct Node RootNode;
    UINT32 Response;

    GetCodecTree(PcieDeviceConfigSpace, &RootNode);

    struct Node *CurrentFunctionGroupNode = NULL;
    struct Node *CurrentWidgetNode = NULL;

    UINT32 CurrentFunctionGroupId = 0;
    UINT32 CurrentFgArrayIndexer = 0;

    UINT32 CurrentWidgetId = 0;
    UINT32 CurrentWidgetArrayIndexer = 0;

    //Sends the command to the root node
    GetCodecData16BitPayloadCorbRirb(PcieDeviceConfigSpace, 0,
									0, Verb,
									VerbPayload, &Response);
    Print(L"Verb 0x%x with payload 0x%x sent to node 0x0\r\n",
        							Verb, VerbPayload);

    for(CurrentFunctionGroupId = RootNode.StartingChildNodeAddess;
    		CurrentFunctionGroupId < RootNode.StartingChildNodeAddess +
    		RootNode.SubordinateNodeCount; CurrentFunctionGroupId++,
    		CurrentFgArrayIndexer++) {

        //Sends the command to the function group node
        GetCodecData16BitPayloadCorbRirb(PcieDeviceConfigSpace, 0,
        		CurrentFunctionGroupId, Verb,
    									VerbPayload, &Response);
        Print(L"Verb 0x%x with payload 0x%x sent to node 0x%x\r\n",
            				Verb, VerbPayload, CurrentFunctionGroupId);

    	//Fills function group information
    	CurrentFunctionGroupNode = &(RootNode.ChildNodes[CurrentFgArrayIndexer]);

    	CurrentWidgetArrayIndexer = 0;

    	for(CurrentWidgetId = CurrentFunctionGroupNode->StartingChildNodeAddess;
    			CurrentWidgetId < CurrentFunctionGroupNode->StartingChildNodeAddess +
    			CurrentFunctionGroupNode->SubordinateNodeCount - 1;
    			CurrentWidgetId++, CurrentWidgetArrayIndexer++) {

    		//fills widget information
    		CurrentWidgetNode = &(CurrentFunctionGroupNode->ChildNodes[CurrentWidgetArrayIndexer]);

    		GetCodecData16BitPayloadCorbRirb(PcieDeviceConfigSpace, 0,
    										CurrentWidgetId, Verb,
											VerbPayload, &Response);

    		Print(L"Verb 0x%x with payload 0x%x sent to node 0x%x\r\n",
    				Verb, VerbPayload, CurrentWidgetId);
    	}
    }

    ReleaseCodecTree(PcieDeviceConfigSpace, &RootNode);

	return Status;
}

/*
 * Since the immediate buffers are not mandatory these functions may
 * not work on some platforms, but well keep them here for reference
 *
 */
/*
EFI_STATUS GetCodecData8BitPayload (PCI_HDA_REGION* PcieDeviceConfigSpace,
		      UINT8 CodecAddress, UINT8 NodeId,
		      HDA_VERB Verb, UINT8 VerbPayload,
		      UINT32 *Response) {

    HDA_COMMAND_FIELD_8BIT_PAYLOAD comando;
	UINT32 VerbToSend = 0;
	UINT32 RegisterNewValue = 0;

	comando.CAd = CodecAddress;
	comando.NID = NodeId;
	comando.VerbIdent = Verb;
	comando.VerbPayload = VerbPayload;

	CopyMem(&VerbToSend, &comando, sizeof(UINT32));

	//TODO: Check if some DMA transaction is happening before
	//write verb
	WriteControllerRegister(PcieDeviceConfigSpace,
							HDA_OFFSET_ICOI,
							(VOID*) &VerbToSend,
							1,
							Bar0,
							EfiPciWidthUint32);

	//start verb processing
	RegisterNewValue = HDA_START_PROCESSING_IMMEDIATE_COMMAND;
	WriteControllerRegister(PcieDeviceConfigSpace,
							HDA_OFFSET_ICIS,
							(VOID*)&RegisterNewValue,
							1,
							Bar0,
							EfiPciWidthUint32);

	//wait processing
	do {
		ReadControllerRegister(PcieDeviceConfigSpace,
				HDA_OFFSET_ICIS, (VOID*) Response,  1, EfiPciWidthUint32);
	} while ((*Response & 0x1) == 0x1);

	//read result
	ReadControllerRegister(PcieDeviceConfigSpace,
			HDA_OFFSET_ICII, (VOID*) Response, 1, EfiPciWidthUint32);

	return EFI_SUCCESS;
}


EFI_STATUS GetCodecData16BitPayload (PCI_HDA_REGION* PcieDeviceConfigSpace,
		      UINT8 CodecAddress, UINT8 NodeId,
		      HDA_VERB Verb, UINT16 VerbPayload,
		      UINT32 *Response) {



    HDA_COMMAND_FIELD_16BIT_PAYLOAD comando;
	UINT32 VerbToSend = 0;
	UINT32 RegisterNewValue = 0;

	comando.CAd = CodecAddress;
	comando.NID = NodeId;
	comando.VerbIdent = Verb;
	comando.VerbPayload = VerbPayload;

	CopyMem(&VerbToSend, &comando, sizeof(UINT32));

	//TODO: Check if some DMA transaction is happening before
	//write verb
	WriteControllerRegister(PcieDeviceConfigSpace,
								HDA_OFFSET_ICOI,
								(VOID*) &VerbToSend,
								1,
								Bar0,
								EfiPciWidthUint32);

	//start verb processing
	RegisterNewValue = HDA_START_PROCESSING_IMMEDIATE_COMMAND;
	WriteControllerRegister(PcieDeviceConfigSpace,
							  HDA_OFFSET_ICIS,
							  (VOID*) &RegisterNewValue,
							  1,
							  Bar0,
							  EfiPciWidthUint32);

	//wait processing
	do {
		ReadControllerRegister(PcieDeviceConfigSpace,
				HDA_OFFSET_ICIS, (VOID*) Response, 1, EfiPciWidthUint32);
	} while ((*Response & 0x1) == 0x1);

	//read result
	ReadControllerRegister(PcieDeviceConfigSpace,
			HDA_OFFSET_ICII, (VOID*) Response, 1, EfiPciWidthUint32);

	return EFI_SUCCESS;
}
*/


/*
 * InputOutput (1 = output amplifier / 0 = input amplifier)
 * LeftRight (1 = left channel / 0 = right channel)
 */
UINT32 GetAmplifierGain(PCI_HDA_REGION *PcieDeviceConfigSpace,
		                 UINT8 NodeId, BOOLEAN InputOutput,
		                 BOOLEAN LeftRight)
{
	UINT32 Response = 0;
	UINT16 PayloadToSend = 0;

	PayloadToSend = NodeId;

	//Set input or output
	PayloadToSend = PayloadToSend | (InputOutput << 15);

	//Set left or right channel
	PayloadToSend = PayloadToSend | (LeftRight << 13);

	GetCodecData8BitPayloadCorbRirb(PcieDeviceConfigSpace, 0x0,
				NodeId, HDA_VRB_GET_AMPLIFIER_GAIN_MUTE,
				0x0,  &Response);

	return Response;

}

EFI_STATUS DisablePcieInterrupts (PCI_HDA_REGION* PcieDeviceConfigSpace)
{
	UINT16 CommandRegister = 0;
	EFI_STATUS Status = EFI_SUCCESS;

	//Read the command register from PCIe config space
	Status = ReadControllerPcieConfiguration(PcieDeviceConfigSpace,
                                               HDA_OFFSET_PCIE_PCICMD,
											   &CommandRegister,
											   1,
											   EfiPciWidthUint16);

	Print(L"Command: %x\r\n", CommandRegister);

	//Set DisableInterrupt bit
	CommandRegister += (1 << 10);

	Print(L"Command: %x\r\n", CommandRegister);

	WriteControllerPcieConfiguration(PcieDeviceConfigSpace,
			                           HDA_OFFSET_PCIE_PCICMD,
                                       (VOID*) &CommandRegister,
									   1,
									   EfiPciWidthUint16);

	return Status;
}

EFI_STATUS EnablePcieNoSnoop (PCI_HDA_REGION* PcieDeviceConfigSpace)
{
	UINT16 DeviceControlRegister = 0;
	EFI_STATUS Status = EFI_SUCCESS;

	//Read the command register from PCIe config space
	Status = ReadControllerPcieConfiguration(PcieDeviceConfigSpace,
                                               HDA_OFFSET_PCIE_DEVCTL,
											   (VOID*) &DeviceControlRegister,
											   1,
											   EfiPciWidthUint16);

	Print(L"DeviceControlRegister: %x\r\n", DeviceControlRegister);

	//Set DisableInterrupt bit
	DeviceControlRegister += (1 << 11);

	Print(L"DeviceControlRegister: %x\r\n", DeviceControlRegister);

	WriteControllerPcieConfiguration(PcieDeviceConfigSpace,
                                       HDA_OFFSET_PCIE_DEVCTL,
									   (VOID*) &DeviceControlRegister,
									   1,
									   EfiPciWidthUint16);

	return Status;
}
