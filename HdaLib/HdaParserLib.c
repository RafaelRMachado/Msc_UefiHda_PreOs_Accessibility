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
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

#include "../HdaLib/HdaLib.h"
#include "../HdaLib/PciConfigurations.h"

EFI_STATUS ParsePcieDeviceData (PCI_HDA_REGION* PcieDeviceConfigSpace) {

 EFI_STATUS Status = EFI_SUCCESS;

 if(PcieDeviceConfigSpace != NULL)
 {
   //Device's information
   Print(L"\r\n****Pcie Device Information****\r\n");
   /*
   Print(L" Vendor Identification : 0x%x\r\n", PcieDeviceConfigSpace-> VID );
   Print(L" Device Identification : 0x%x\r\n", PcieDeviceConfigSpace-> DID );

   Print(L" PCI Comand : 0x%x\r\n", PcieDeviceConfigSpace-> PCICMD);
   Print(L"    I/O Space Enable: 0x%x\r\n", PcieDeviceConfigSpace-> PCICMD & 0x1);
   Print(L"    Memory Space Enable: 0x%x\r\n", (PcieDeviceConfigSpace-> PCICMD >> 1) & 0x1);
   Print(L"    Bus Master Enable: 0x%x\r\n", (PcieDeviceConfigSpace-> PCICMD >> 2) & 0x1);
   Print(L"    Special Cycle Enable: 0x%x\r\n", (PcieDeviceConfigSpace-> PCICMD >> 3) & 0x1);
   Print(L"    Memory Write and Invalidate Enable: 0x%x\r\n", (PcieDeviceConfigSpace-> PCICMD >> 4) & 0x1);
   Print(L"    VGA Palette Snoop: 0x%x\r\n", (PcieDeviceConfigSpace-> PCICMD >> 5) & 0x1);
   Print(L"    Parity Error Response: 0x%x\r\n", (PcieDeviceConfigSpace-> PCICMD >> 6) & 0x1);
   Print(L"    Wait Cycle Control: 0x%x\r\n", (PcieDeviceConfigSpace-> PCICMD >> 7) & 0x1);
   Print(L"    SERR Enable: 0x%x\r\n", (PcieDeviceConfigSpace-> PCICMD >> 8) & 0x1);
   Print(L"    Fast Back to Back Enable: 0x%x\r\n", (PcieDeviceConfigSpace-> PCICMD >> 9) & 0x1);
   Print(L"    Interrupt Disable: 0x%x\r\n", (PcieDeviceConfigSpace-> PCICMD >> 10) & 0x1);
*/

   Print(L" PCI Status: 0x%x\r\n", PcieDeviceConfigSpace->PCISTS);
   Print(L"    Interrupt Status : 0x%x\r\n", (PcieDeviceConfigSpace->PCISTS >> 3) & 0x1);
   Print(L"    Capabilities List : 0x%x\r\n", (PcieDeviceConfigSpace->PCISTS >> 4) & 0x1);
   Print(L"    Received Master Abort : 0x%x\r\n", (PcieDeviceConfigSpace->PCISTS >> 13) & 0x1);

/*
   Print(L" Revision Identification : 0x%x\r\n", PcieDeviceConfigSpace-> RID );
   Print(L" Programming Interface : 0x%x\r\n", PcieDeviceConfigSpace-> PI );
   Print(L" Sub Class Code : 0x%x\r\n", PcieDeviceConfigSpace->SC );
   Print(L" Base Class Code : 0x%x\r\n", PcieDeviceConfigSpace-> BCC );
   Print(L" Cache Line Size : 0x%x\r\n", PcieDeviceConfigSpace-> CLS );
   Print(L" Latency Timer : 0x%x\r\n", PcieDeviceConfigSpace-> LT );
   Print(L" header Type : 0x%x\r\n", PcieDeviceConfigSpace-> HEADTYP );
   Print(L" Hda Lower Base Address : 0x%x\r\n", PcieDeviceConfigSpace-> HDBARL );
   Print(L" Hda Upper Base Address : 0x%x\r\n", PcieDeviceConfigSpace-> HDBARU );
   Print(L" Sybsystem Vendor Identification : 0x%x\r\n", PcieDeviceConfigSpace-> SVID);
   Print(L" Subsystem Identification : 0x%x\r\n", PcieDeviceConfigSpace-> SID );
   Print(L" Capability List Pointer : 0x%x\r\n", PcieDeviceConfigSpace-> CAPPTR );
   Print(L" Interrupt Line : 0x%x\r\n", PcieDeviceConfigSpace-> INTLN );
   Print(L" Interrupt Pin : 0x%x\r\n", PcieDeviceConfigSpace-> INTPN );

   Print(L" HDA Control : 0x%x\r\n", PcieDeviceConfigSpace->HDCTL);
   Print(L"    HDA/AC97 : 0x%x\r\n", PcieDeviceConfigSpace->HDCTL & 0x1);
   Print(L"    BITCLK Detect Inverted : 0x%x\r\n", (PcieDeviceConfigSpace->HDCTL >> 1) & 0x1);
   Print(L"    BITCLK Detect Enable : 0x%x\r\n", (PcieDeviceConfigSpace->HDCTL >> 2) & 0x1);
   Print(L"    BITCLK Detect Clear : 0x%x\r\n", (PcieDeviceConfigSpace->HDCTL >> 3) & 0x1);


   Print(L" Traffic Class Select : 0x%x\r\n", PcieDeviceConfigSpace->TCSEL);
   Print(L"    Traffic Class Select : TC%d\r\n", PcieDeviceConfigSpace->TCSEL & 0x7);

   Print(L" Docking Status : 0x%x\r\n", PcieDeviceConfigSpace-> DCKSTS );
   Print(L"    Bit7: 0x%x (Bios is REQUIRED to clear this bit)\r\n",
                                  (PcieDeviceConfigSpace->DCKSTS >> 7) & 0x1);


   Print(L" PCI Power Management Cap Id : 0x%x\r\n", PcieDeviceConfigSpace->PID  & 0xFF );
   Print(L" Power Management Cap : 0x%x\r\n", PcieDeviceConfigSpace->PC );
   Print(L"  Version : 0x%x\r\n", PcieDeviceConfigSpace->PC & 0x3);
   Print(L"  PME Clock : 0x%x (need clk?)\r\n", (PcieDeviceConfigSpace->PC >> 3) & 0x1);
   Print(L"  DSI (need Device-specific init?): 0x%x\r\n", (PcieDeviceConfigSpace->PC >> 5) & 0x1);
   Print(L"  AUX Current : 0x%x\r\n", (PcieDeviceConfigSpace->PC >> 6 ) & 0x7);
   Print(L"   0 = 0 mA\r\n");
   Print(L"   1 = 55 mA\r\n");
   Print(L"   2 = 100 mA\r\n");
   Print(L"   3 = 160 mA\r\n");
   Print(L"   4 = 220 mA\r\n");
   Print(L"   5 = 270 mA\r\n");
   Print(L"   6 = 320 mA\r\n");
   Print(L"   7 = 375 mA\r\n");
   Print(L"  D1 Support? : 0x%x\r\n", (PcieDeviceConfigSpace->PC >> 9) & 0x1);
   Print(L"  D2 Support? : 0x%x\r\n", (PcieDeviceConfigSpace->PC >> 10) & 0x1);
   Print(L"  PME Support : 0x%x\r\n", (PcieDeviceConfigSpace->PC >> 11) & 0x1F);
   Print(L"      Generate PME at D0?: %d\r\n", (PcieDeviceConfigSpace->PC >> 11) & 0x1);
   Print(L"      Generate PME at D1: %d\r\n", (PcieDeviceConfigSpace->PC >> 12) & 0x1);
   Print(L"      Generate PME at D2: %d\r\n", (PcieDeviceConfigSpace->PC >> 13) & 0x1);
   Print(L"      Generate PME at D3 hot: %d\r\n", (PcieDeviceConfigSpace->PC >> 14) & 0x1);
   Print(L"      Generate PME at D3 cold: %d\r\n", (PcieDeviceConfigSpace->PC >> 15) & 0x1);
   Print(L" Power Management Control Status : 0x%x\r\n", PcieDeviceConfigSpace->PCS);
   Print(L"      Power State : 0x%x\r\n", PcieDeviceConfigSpace->PCS & 0x3);
   Print(L"          0 = D0\r\n");
   Print(L"          1 = D1\r\n");
   Print(L"          2 = D2\r\n");
   Print(L"          3 = D3\r\n");
   Print(L"      PME Enable : 0x%x\r\n", (PcieDeviceConfigSpace->PCS >> 8) & 0x1);
   Print(L"      Data Select : 0x%x\r\n", (PcieDeviceConfigSpace->PCS >> 9) & 0xF);
   Print(L"      Data Scale : 0x%x\r\n", (PcieDeviceConfigSpace->PCS >> 13) & 0x3);
   Print(L"      PME Status : 0x%x\r\n", (PcieDeviceConfigSpace->PCS >> 15) & 0x1);


   Print(L" MSI Cap Id : 0x%x\r\n", PcieDeviceConfigSpace-> MID & 0xFF);
   Print(L" MSI Message Control : 0x%x\r\n", PcieDeviceConfigSpace->MMC);
   Print(L"     MSI Enable : 0x%x\r\n", PcieDeviceConfigSpace->MMC & 0x1);
   Print(L"     Multiple Message Capable : 0x%x\r\n", (PcieDeviceConfigSpace->MMC >> 1) & 0x7);
   Print(L"         0 = 1 message\r\n");
   Print(L"     Multiple Message Enable (hardwired to 0) : 0x%x\r\n", (PcieDeviceConfigSpace->MMC >> 4) & 0x7);
   Print(L"     64bit address capability : 0x%x\r\n", (PcieDeviceConfigSpace->MMC >> 7) & 0x1);
   Print(L"         This means if it can generate 64 bit message address\r\n");
   Print(L" MSI Message Lower Address : 0x%x\r\n",PcieDeviceConfigSpace-> MMLA);
   Print(L" MSI Message Upper Address : 0x%x\r\n",PcieDeviceConfigSpace-> MMUA);
   Print(L" MSI Message Data : 0x%x\r\n",PcieDeviceConfigSpace-> MMD);

   Print(L" PCIe Cap Id : 0x%x\r\n",PcieDeviceConfigSpace-> PXID &  0xFF);
   Print(L" PCIe Cap : 0x%x\r\n",PcieDeviceConfigSpace-> PXC );

   Print(L" Dev Cap : 0x%x\r\n", PcieDeviceConfigSpace->DEVCAP);
   Print(L"    End Point L0s Acceptable Latency : 0x%x\r\n",
                                     (PcieDeviceConfigSpace->DEVCAP >> 6) & 0x7);
   Print(L"    End Point L1s Acceptable Latency : 0x%x\r\n",
                                    (PcieDeviceConfigSpace->DEVCAP >> 9) & 0x7);


   Print(L" Dev Control: 0x%x\r\n", PcieDeviceConfigSpace-> DEVCTL);
   Print(L"    No Snoop Enable: 0x%x\r\n", (PcieDeviceConfigSpace->DEVCTL >> 11) & 0x1);

   Print(L" Dev Status : 0x%x\r\n", PcieDeviceConfigSpace->DEVS);
   Print(L"    Aux Power Detected : 0x%x\r\n", (PcieDeviceConfigSpace->DEVS >> 0x4) & 0x1);
   Print(L"    Transaction Pending : 0x%x\r\n", (PcieDeviceConfigSpace->DEVS >> 0x5) & 0x1);

   Print(L" Virtual Channel Enhanced Cap Header : 0x%x\r\n", PcieDeviceConfigSpace->VCCAP);
   Print(L" Port VC Cap Reg 1 : 0x%x\r\n", PcieDeviceConfigSpace-> PVCCAP1 );
   Print(L" Port VC Cap Reg 2 : 0x%x\r\n", PcieDeviceConfigSpace-> PVCCAP2 );
   Print(L" Port VC Control : 0x%x\r\n", PcieDeviceConfigSpace-> PVCCTL);
   Print(L" Port VC Status : 0x%x (If this is 0 no Arbitration table is present)\r\n",
                                                             PcieDeviceConfigSpace->PVCSTS);
   Print(L" VC0 Res Cap : 0x%x\r\n", PcieDeviceConfigSpace->VC0CAP);
   Print(L" VC0 Res Control : 0x%x\r\n", PcieDeviceConfigSpace->VC0CTL);
   Print(L"    TC/VC0 Map: 0x%x\r\n", PcieDeviceConfigSpace->VC0CTL & 0xFF);
   Print(L"    VC0 ID: 0x%x\r\n", (PcieDeviceConfigSpace->VC0CTL >> 24) & 0x3);
   Print(L"    VC0 Enable: 0x%x\r\n", (PcieDeviceConfigSpace->VC0CTL >> 31) & 0x1);
   Print(L" VC0 Res Status : 0x%x\r\n", PcieDeviceConfigSpace->VC0STS);

   Print(L" Vci Res Cap : 0x%x\r\n", PcieDeviceConfigSpace-> VciCAP);
   Print(L" Vci Res Control: 0x%x\r\n", PcieDeviceConfigSpace->VciCTL);
   Print(L"    TC/VCi Map: 0x%x\r\n", PcieDeviceConfigSpace->VciCTL & 0xFF);
   Print(L"    VCi ID: 0x%x\r\n", (PcieDeviceConfigSpace->VciCTL >> 24) & 0x3);
   Print(L"    VCi Enable: 0x%x\r\n", (PcieDeviceConfigSpace->VciCTL >> 31) & 0x1);

   Print(L" Vci Res Status : 0x%x\r\n", PcieDeviceConfigSpace->VciSTS);
   Print(L" Root Complex Link Dec Enh Cap Header: 0x%x\r\n", PcieDeviceConfigSpace->RCCAP);
   Print(L" Element Self Description: 0x%x\r\n", PcieDeviceConfigSpace->ESD);
   Print(L" Link 1 Description: 0x%x\r\n", PcieDeviceConfigSpace->L1DESC);
   Print(L" Link 1 Lower Address: 0x%x\r\n", PcieDeviceConfigSpace->L1ADDL);
   Print(L" Link 1 Upper Address: 0x%x\r\n", PcieDeviceConfigSpace->L1ADDU);
*/

   Print(L"\r\n*******************************\r\n");
 } else {
  Status = EFI_INVALID_PARAMETER;
 }

 return Status;
}


VOID ParseStreamDescriptor (HDA_CONTROLLER_STREAM_DESCRIPTOR StreamDescriptor) {

//  UINT32* DescriptorData = (UINT32*) &(StreamDescriptor.SD0CTL[0]);
/*
  //Input Stream Descriptors
  Print(L"   Stream Control: 0x%x\r\n", *DescriptorData);

  Print(L"      Stream Reset (SRST): 0x%x\r\n", *DescriptorData & 0x1);
  Print(L"      Stream Run (RUN): 0x%x\r\n", (*DescriptorData >> 1) & 0x1);
  Print(L"      Interrupt On Completion Enable (IOCE): 0x%x\r\n",
    (*DescriptorData >> 2) & 0x1);
  Print(L"      FIFO Error Interrupt Enable (FEIE): 0x%x\r\n",
    (*DescriptorData >> 3) & 0x1);
  Print(L"      Descriptor Error Interrupt Enable (DEIE): 0x%x\r\n",
    (*DescriptorData >> 4) & 0x1);
  Print(L"      Stripe Control (STRIPE): 0x%x\r\n", (*DescriptorData >> 16) & 0x2);
  Print(L"      Traffic Priority (TP): 0x%x\r\n", (*DescriptorData >> 18) & 0x1);
  Print(L"      Bidirectional Direction Control (DIR): 0x%x\r\n",
    (*DescriptorData >> 19) & 0x1);
  Print(L"      Stream Number (STRM): 0x%x\r\n",
    (*DescriptorData >> 20) & 0xF);
*/
  Print(L"   Stream Descriptor Status (SD0STS): 0x%x\r\n",
    StreamDescriptor.SD0STS);
  Print(L"      Buffer Completion Interrupt Status (BCIS): 0x%x\r\n",
    (StreamDescriptor.SD0STS >> 2) & 0x1);
  Print(L"      FIFO Error (FIFOE): 0x%x\r\n",
    (StreamDescriptor.SD0STS >> 3) & 0x1);
  Print(L"      Descriptor Error (DESE): 0x%x\r\n",
    (StreamDescriptor.SD0STS >> 4) & 0x1);
  Print(L"      FIFO Ready (FIFORDY): 0x%x\r\n",
    (StreamDescriptor.SD0STS >> 5) & 0x1);


  Print(L"   Link POsition in Buffer (SD0LPIB): 0x%x\r\n",
    StreamDescriptor.SD0LPIB);
/*
  Print(L"   Cyclic Buffer Length (CBL): 0x%x\r\n",
    StreamDescriptor.SD0CBL);

  Print(L"   Last Valid Index (SD0LVI): 0x%x\r\n",
    StreamDescriptor.SD0LVI & 0xFF);

  Print(L"   FIFO Size (FIFOS): 0x%x (%d) bytes\r\n",
    StreamDescriptor.SD0FIFOS, StreamDescriptor.SD0FIFOS);
*/
  Print(L"   Stream Format (SD0FMT): 0x%x\r\n",
    StreamDescriptor.SD0FMT);

/*
 Print(L"      Number Of Channels (CHAN): 0x%x\r\n",
    StreamDescriptor.SD0FMT & 0xF);
  Print(L"      	0: 1 channel\r\n");
  Print(L"      	1: 2 channels\r\n");
  Print(L"      	1111: 16 channels\r\n");

  switch ((StreamDescriptor.SD0FMT >> 4) & 0x7){
   case 0:
    Print(L"      Bits Per Sample (BITS): 8 bits\r\n");
    break;
   case 1:
    Print(L"      Bits Per Sample (BITS): 16 bits\r\n");
    break;
   case 2:
    Print(L"      Bits Per Sample (BITS): 20 bits\r\n");
    break;
   case 3:
    Print(L"      Bits Per Sample (BITS): 24 bits\r\n");
    break;
   case 4:
    Print(L"      Bits Per Sample (BITS): 32 bits\r\n");
    break;
  }

  switch ((StreamDescriptor.SD0FMT >> 8) & 0x7){
  case 0:
   Print(L"      Sample Base Rate Divisor (DIV): Divide by 1 (48 kHz, 44.1 kHz\r\n");
   break;
  case 1:
   Print(L"      Sample Base Rate Divisor (DIV): Divide by 2 (24 kHz, 22.05 kHz\r\n");
   break;
  case 2:
   Print(L"      Sample Base Rate Divisor (DIV): Divide by 3 (16 kHz, 32 kHz\r\n");
   break;
  case 3:
   Print(L"      Sample Base Rate Divisor (DIV): Divide by 4 (11.025 kHz\r\n");
   break;
  case 4:
   Print(L"      Sample Base Rate Divisor (DIV): Divide by 5 (9.6 kHz\r\n");
   break;
  case 5:
   Print(L"      Sample Base Rate Divisor (DIV): Divide by 6 (8 kHz\r\n");
   break;
  case 6:
   Print(L"      Sample Base Rate Divisor (DIV): Divide by 7\r\n");
   break;
  case 7:
   Print(L"      Sample Base Rate Divisor (DIV): Divide by 8 (6 kHz\r\n");
   break;
  }

  switch ((StreamDescriptor.SD0FMT >> 11) & 0x7){
  case 0:
   Print(L"      Sample Base Rate Multiple (MULT): 48 kHz/44.1 kHz or less\r\n");
   break;
  case 1:
   Print(L"      Sample Base Rate Multiple (MULT): x2 (96 kHz, 88.2 kHz, 32 kHz)\r\n");
   break;
  case 2:
   Print(L"      Sample Base Rate Multiple (MULT): x3 (144 kHz)\r\n");
   break;
  case 3:
   Print(L"      Sample Base Rate Multiple (MULT):  x4 (192 kHz, 176.4 kHz)\r\n");
   break;
  }

  switch ((StreamDescriptor.SD0FMT >> 14) & 0x1){
  case 0:
   Print(L"      Sample Base Rate (BASE): 48 kHz\r\n");
   break;
  case 1:
   Print(L"      Sample Base Rate (BASE): 44.1 kHz\r\n");
   break;
  }

*/

  Print(L"   Buffer Descriptor List Lower Base Address (BDLLBASE): 0x%x\r\n",
    StreamDescriptor.SD0BDPL);

  Print(L"   Buffer Descriptor List Upper Base Address (SD0BDPU): 0x%x\r\n",
    StreamDescriptor.SD0BDPU);


}


EFI_STATUS ParseControllerRegisterSet (HDA_CONTROLLER_REGISTER_SET* ControllerRegisterSet) {

 EFI_STATUS Status = EFI_SUCCESS;
 UINTN Count = 0;

 UINTN IssCount = (ControllerRegisterSet->GCAP >> 8) & 0xF;
 UINTN OssCount = (ControllerRegisterSet->GCAP >> 12) & 0xF;
// UINTN BssCount = (ControllerRegisterSet->GCAP >> 3) & 0x1F;
// UINTN CORBSize = ControllerRegisterSet->CORBSIZE & 0x3;
// UINTN CORBSizeCapability = ControllerRegisterSet->CORBSIZE >> 4;
// UINTN RIRBSize = ControllerRegisterSet->RIRBSIZE & 0x3;
// UINTN RIRBSizeCapability = ControllerRegisterSet->RIRBSIZE >> 4;

 if(ControllerRegisterSet != NULL) {

   //Pag 30 of intel high definition spec
/*
   Print(L"\r\n****HDA Controller Register Set****\r\n");
   Print(L"Global Capabilities (GCAP): 0x%x\r\n",
     ControllerRegisterSet->GCAP);
   Print(L"   64 Bit address Supported (64OK): %d\r\n",
        ControllerRegisterSet->GCAP & 0x1);
   Print(L"   Number of Serial Data Out Signals (NSDO): %d\r\n",
     (ControllerRegisterSet->GCAP >> 1 ) & 0x3);

   Print(L"   Number of Bidirectional Streams Supported (BSS): %d\r\n",
        (ControllerRegisterSet->GCAP >> 3) & 0x1F);

   Print(L"   Number of Input Streams Supported (ISS): %d\r\n",
     (ControllerRegisterSet->GCAP >> 8) & 0xF);

   Print(L"   Number of Output Streams Supported (OSS): %d\r\n",
     (ControllerRegisterSet->GCAP >> 12) & 0xF);

   Print(L"Minor Version (VMIN): 0x%x\r\n", ControllerRegisterSet->VMIN);
   Print(L"Major Version (VMAJ): 0x%x\r\n", ControllerRegisterSet->VMAJ);

   Print(L"Output Payload Capabilities (OUTPAY):"
     " %d (16 bit words)\r\n", ControllerRegisterSet->OUTPAY);

   Print(L"Input Payload capabilities (INPAY):"
     " %d (16 bit words)\r\n", ControllerRegisterSet->INPAY);

   Print(L"Global Control (GCTL): 0x%x\r\n",
     ControllerRegisterSet->GCTL);
   Print(L"   Controller Reset (CRST): %d\r\n",
     ControllerRegisterSet->GCTL & 0x1);
   Print(L"   Flush Control (FCNTRL): %d\r\n",
     (ControllerRegisterSet->GCTL >> 1) & 0x1);
   Print(L"   Accept Unsolicited Response Enable (UNSOL): %d\r\n",
     (ControllerRegisterSet->GCTL >> 8) & 0x1);
*/
   Print(L"Global Status (GSTS): 0x%x\r\n",
        ControllerRegisterSet->GSTS);
   Print(L"   Flush Status (FSTS): %d\r\n",
     (ControllerRegisterSet->GSTS >> 1) & 0x1);
/*
   Print(L"Wake Enable (WAKEEN): 0x%x\r\n", ControllerRegisterSet->WAKEEN);
   Print(L"   SDATA_IN[0]: 0x%x\r\n", ControllerRegisterSet->WAKEEN & 0x1);
   Print(L"   SDATA_IN[1]: 0x%x\r\n", (ControllerRegisterSet->WAKEEN >> 1) & 0x1);
   Print(L"   SDATA_IN[2]: 0x%x\r\n", (ControllerRegisterSet->WAKEEN >> 2) & 0x1);
   Print(L"   SDATA_IN[3]: 0x%x\r\n", (ControllerRegisterSet->WAKEEN >> 3) & 0x1);
   Print(L"   SDATA_IN[4]: 0x%x\r\n", (ControllerRegisterSet->WAKEEN >> 4) & 0x1);
   Print(L"   SDATA_IN[5]: 0x%x\r\n", (ControllerRegisterSet->WAKEEN >> 5) & 0x1);
   Print(L"   SDATA_IN[6]: 0x%x\r\n", (ControllerRegisterSet->WAKEEN >> 6) & 0x1);
   Print(L"   SDATA_IN[7]: 0x%x\r\n", (ControllerRegisterSet->WAKEEN >> 7) & 0x1);
   Print(L"   SDATA_IN[8]: 0x%x\r\n", (ControllerRegisterSet->WAKEEN >> 8) & 0x1);
   Print(L"   SDATA_IN[9]: 0x%x\r\n", (ControllerRegisterSet->WAKEEN >> 9) & 0x1);
   Print(L"   SDATA_IN[10]: 0x%x\r\n", (ControllerRegisterSet->WAKEEN >> 10) & 0x1);
   Print(L"   SDATA_IN[11]: 0x%x\r\n", (ControllerRegisterSet->WAKEEN >> 11) & 0x1);
   Print(L"   SDATA_IN[12]: 0x%x\r\n", (ControllerRegisterSet->WAKEEN >> 12) & 0x1);
   Print(L"   SDATA_IN[13]: 0x%x\r\n", (ControllerRegisterSet->WAKEEN >> 13) & 0x1);
   Print(L"   SDATA_IN[14]: 0x%x\r\n", (ControllerRegisterSet->WAKEEN >> 14) & 0x1);
   Print(L"   SDATA_IN[15]: 0x%x\r\n", (ControllerRegisterSet->WAKEEN >> 15) & 0x1);


   Print(L"Wake State Change Status (STATESTS): 0x%x\r\n", ControllerRegisterSet->STATESTS);
   Print(L"   SDATA_IN[0]: 0x%x\r\n", ControllerRegisterSet->STATESTS & 0x1);
   Print(L"   SDATA_IN[1]: 0x%x\r\n", (ControllerRegisterSet->STATESTS >> 1) & 0x1);
   Print(L"   SDATA_IN[2]: 0x%x\r\n", (ControllerRegisterSet->STATESTS >> 2) & 0x1);
   Print(L"   SDATA_IN[3]: 0x%x\r\n", (ControllerRegisterSet->STATESTS >> 3) & 0x1);
   Print(L"   SDATA_IN[4]: 0x%x\r\n", (ControllerRegisterSet->STATESTS >> 4) & 0x1);
   Print(L"   SDATA_IN[5]: 0x%x\r\n", (ControllerRegisterSet->STATESTS >> 5) & 0x1);
   Print(L"   SDATA_IN[6]: 0x%x\r\n", (ControllerRegisterSet->STATESTS >> 6) & 0x1);
   Print(L"   SDATA_IN[7]: 0x%x\r\n", (ControllerRegisterSet->STATESTS >> 7) & 0x1);
   Print(L"   SDATA_IN[8]: 0x%x\r\n", (ControllerRegisterSet->STATESTS >> 8) & 0x1);
   Print(L"   SDATA_IN[9]: 0x%x\r\n", (ControllerRegisterSet->STATESTS >> 9) & 0x1);
   Print(L"   SDATA_IN[10]: 0x%x\r\n", (ControllerRegisterSet->STATESTS >> 10) & 0x1);
   Print(L"   SDATA_IN[11]: 0x%x\r\n", (ControllerRegisterSet->STATESTS >> 11) & 0x1);
   Print(L"   SDATA_IN[12]: 0x%x\r\n", (ControllerRegisterSet->STATESTS >> 12) & 0x1);
   Print(L"   SDATA_IN[13]: 0x%x\r\n", (ControllerRegisterSet->STATESTS >> 13) & 0x1);
   Print(L"   SDATA_IN[14]: 0x%x\r\n", (ControllerRegisterSet->STATESTS >> 14) & 0x1);
   Print(L"   SDATA_IN[15]: 0x%x\r\n", (ControllerRegisterSet->STATESTS >> 15) & 0x1);

   Print(L"Output Stream Payload Capability (OUTSTRMPAY): %d "
     "(Maximum of 16bit words per frame)\r\n", ControllerRegisterSet->OUTSTRMPAY);

   Print(L"Input Stream Payload Capability (INSTRMPAY): %d "
     "(Maximum of 16bit words per frame)\r\n", ControllerRegisterSet->INSTRMPAY);


   Print(L"Interrupt Control (INTCTL): 0x%x\r\n", ControllerRegisterSet->INTCTL);
   for(Count = 0; Count < IssCount; Count++) {
    Print(L"   Input Stream %d Interrupt Enable (SIE): %d\r\n", Count,
      (ControllerRegisterSet->INTCTL >> Count) & 0x1);
   }
   for(Count = 0; Count < OssCount; Count++) {
    Print(L"   Output Stream %d Interrupt Enable (SIE): %d\r\n", Count,
      (ControllerRegisterSet->INTCTL >> (IssCount + Count)) & 0x1);
   }
   for(Count = 0; Count < BssCount; Count++) {
    Print(L"   Bidirecional Stream %d Interrupt Enable (SIE): %d\r\n", Count,
      (ControllerRegisterSet->INTCTL >> (IssCount + OssCount + Count)) & 0x1);
   }
   Print(L"   Controller Interrupt Enable (CIE) : %d\r\n",
     (ControllerRegisterSet->INTCTL >> 30) & 0x1);
   Print(L"   Global Interrupt Enable (GIE) : %d\r\n",
     (ControllerRegisterSet->INTCTL >> 31) & 0x1);
*/

   Print(L"Interrupt Status (INTSTS): 0x%x\r\n", ControllerRegisterSet->INTSTS);
/*
   for(Count = 0; Count < IssCount; Count++) {
    Print(L"   Input Stream %d Interrupt Status (SIS): %d\r\n", Count,
      (ControllerRegisterSet->INTSTS >> Count) & 0x1);
   }
   */
   for(Count = 0; Count < OssCount; Count++) {
    Print(L"   Output Stream %d Interrupt Status (SIS): %d\r\n", Count,
      (ControllerRegisterSet->INTSTS >> (IssCount + Count)) & 0x1);
   }
   /*
   for(Count = 0; Count < BssCount; Count++) {
    Print(L"   Bidirecional Stream %d Interrupt Status (SIS): %d\r\n", Count,
      (ControllerRegisterSet->INTSTS >> (IssCount + OssCount + Count)) & 0x1);
   }
   */
   Print(L"   Controller Interrupt Status (CIS) : %d\r\n",
     (ControllerRegisterSet->INTSTS >> 30) & 0x1);
   Print(L"   Global Interrupt Status (GIS) : %d\r\n",
     (ControllerRegisterSet->INTSTS >> 31) & 0x1);

/*
   Print(L"Wall Clock Counter - 24-MHz rate (WALCLK): 0x%x\r\n", ControllerRegisterSet->WALCLK);


   Print(L"Stream Synchronization (SSYNC): 0x%x\r\n", ControllerRegisterSet->SSYNC);
   for(Count = 0; Count < IssCount; Count++) {
    Print(L"   Input Stream %d Synchronization Start: %d\r\n", Count,
      (ControllerRegisterSet->SSYNC >> Count) & 0x1);
   }
   for(Count = 0; Count < OssCount; Count++) {
    Print(L"   Output Stream %d  Synchronization Start: %d\r\n", Count,
      (ControllerRegisterSet->SSYNC >> (IssCount + Count)) & 0x1);
   }
   for(Count = 0; Count < BssCount; Count++) {
    Print(L"   Bidirecional Stream %d Synchronization Start: %d\r\n", Count,
      (ControllerRegisterSet->SSYNC >> (IssCount + OssCount + Count)) & 0x1);
   }


   Print(L"\nCORB - Command Output Ring Buffer\r\n");
   Print(L"CORBLBASE: 0x%x\r\n", ControllerRegisterSet->CORBLBASE);
   Print(L"   CORBLBASE Unimplemented Bits: 0x%x\r\n",
     ControllerRegisterSet->CORBLBASE & 0x7F);
   Print(L"   CORB Lower Base Address (CORBLBASE): 0x%x\r\n",
        (ControllerRegisterSet->CORBLBASE >> 7));

   Print(L"CORB Upper Base Address (CORBUBASE): 0x%x\r\n",
     ControllerRegisterSet->CORBUBASE);

   Print(L"CORB Write Pointer (CORBWP): 0x%x\r\n",
     ControllerRegisterSet->CORBWP & 0x7F);

   Print(L"CORB Read Pointer: 0x%x\r\n",
     ControllerRegisterSet->CORBRP);
   Print(L"   CORB Read Pointer (CORBRP): 0x%x\r\n",
     ControllerRegisterSet->CORBRP & 0x7F);
   Print(L"   CORB Read Pointer Reset (CORBRPRST): %d\r\n",
     (ControllerRegisterSet->CORBRP >> 15));


   Print(L"CORB Control (CORBCTL): 0x%x\r\n",
     ControllerRegisterSet->CORBCTL);
   Print(L"   CORB Memory Interrupt Enable (CMEIE): %d\r\n",
     ControllerRegisterSet->CORBCTL & 0x1);
   Print(L"   Enable CORB DMA Engine (CORBRUN): %d\r\n",
     (ControllerRegisterSet->CORBCTL >> 1) & 0x1);


   Print(L"CORB Status (CORBSTS): 0x%x\r\n", ControllerRegisterSet->CORBSTS);
   Print(L"   CORB Memory Error Indication (CMEI): %d\r\n",
     ControllerRegisterSet->CORBSTS & 0x1);


   Print(L"CORB Size (CORBSIZE): 0x%x\r\n", ControllerRegisterSet->CORBSIZE);
   if(CORBSize == 0) {
    Print(L"   CORB Size (CORBSIZE): %d (8B = 2 entries)\r\n",
      CORBSize);
   }
   if(CORBSize == 1) {
    Print(L"   CORB Size (CORBSIZE): %d (64B = 16 entries)\r\n",
      CORBSize);
   }
   if(CORBSize == 2) {
    Print(L"   CORB Size (CORBSIZE): %d (1KB = 256 entries)\r\n",
      CORBSize);
   }
   if(CORBSizeCapability & 0x1) {
    Print(L"   CORB Size Capability (CORBSZCAP): %d (8B = 2 entries)\r\n",
      CORBSizeCapability);
   }
   if(CORBSizeCapability & 0x2) {
    Print(L"   CORB Size Capability (CORBSZCAP): %d (64B = 16 entries)\r\n",
      CORBSizeCapability);
   }
   if(CORBSizeCapability & 0x4) {
    Print(L"   CORB Size Capability (CORBSZCAP): %d (1KB = 256 entries)\r\n",
      CORBSizeCapability);
   }


   Print(L"\nRIRB - Response Inbound Ring Buffer\r\n");

   Print(L"RIRBLBASE: 0x%x\r\n", ControllerRegisterSet->RIRBLBASE);
   Print(L"   RIRBLBASE Unimplemented Bits: 0x%x\r\n",
     ControllerRegisterSet->RIRBLBASE & 0x7F);
   Print(L"   RIRB Lower Base Address (RIRBLBASE): 0x%x\r\n",
        (ControllerRegisterSet->RIRBLBASE >> 7));

   Print(L"RIRB Upper Base Address (RIRBUBASE): 0x%x\r\n",
     ControllerRegisterSet->RIRBUBASE);

   Print(L"RIRB Write Pointer (RIRBWP): 0x%x\r\n",
     ControllerRegisterSet->RIRBWP);
   Print(L"  RIRB Write Pointer (RIRBWP): 0x%x\r\n",
     ControllerRegisterSet->RIRBWP & 0x7F);
   Print(L"   RIRB Write Pointer reset (RIRBWPRST): %d\r\n",
     ControllerRegisterSet->RIRBWP >> 15);


   Print(L"Response Interrupt Count (RINTCNT): %d\r\n",
     ControllerRegisterSet->RINTCNT & 0x7F);

   Print(L"RIRB Control (RIRBCTL): 0x%x\r\n", ControllerRegisterSet->RIRBCTL);
   Print(L"   Response Interrupt Control (RINTCTL): %d\r\n",
     ControllerRegisterSet->RIRBCTL & 0x1);
   Print(L"   RIRB DMA Enable (RIRBDMAEN): %d\r\n",
     (ControllerRegisterSet->RIRBCTL >> 1) & 0x1);
   Print(L"   Response Overrun Interrupt Control (RIRBOIC): %d\r\n",
     (ControllerRegisterSet->RIRBCTL >> 2) & 0x1);


   Print(L"RIRB Status (RIRBSTS): 0x%x\r\n",
     ControllerRegisterSet->RIRBSTS);
   Print(L"   Response Interrupt (RINTFL): %d\r\n",
        ControllerRegisterSet->RIRBSTS & 0x1);
   Print(L"   Response Overrun Interrupt Status (RIRBOIS): %d\r\n",
        (ControllerRegisterSet->RIRBSTS >> 2) & 0x1);


   Print(L"RIRB Size (RIRBSIZE): 0x%x\r\n", ControllerRegisterSet->RIRBSIZE);
   if(RIRBSize == 0) {
     Print(L"   RIRB Size (RIRBSIZE): %d (16B = 2 entries)\r\n",
       RIRBSize);
   }
   if(RIRBSize == 1) {
     Print(L"   RIRB Size (RIRBSIZE): %d (128B = 16 entries)\r\n",
       RIRBSize);
   }
   if(RIRBSize == 2) {
     Print(L"   RIRB Size (RIRBSIZE): %d (2KB = 256 entries)\r\n",
       RIRBSize);
   }
   if(RIRBSizeCapability & 0x1) {
     Print(L"   RIRB Size Capability (RIRBSZCAP): %d (16B = 2 entries)\r\n",
       RIRBSizeCapability);
   }
   if(RIRBSizeCapability & 0x2) {
     Print(L"   RIRB Size Capability (RIRBSZCAP): %d (128B = 16 entries)\r\n",
       RIRBSizeCapability);
   }
   if(RIRBSizeCapability & 0x4) {
     Print(L"   RIRB Size Capability (RIRBSZCAP): %d (2048KB = 256 entries)\r\n",
       RIRBSizeCapability);
   }
*/
/*
   Print(L"\nDMA Position Lower Base Address (DPIBLBASE): 0x%x\r\n",
     ControllerRegisterSet->DPIBLBASE);
   Print(L"   DMA Position Buffer Enable: %d\r\n",
        ControllerRegisterSet->DPIBLBASE & 0x1);

   //As defined at the HDA spec:
   // "The lower 7 bits of the DMA Position Buffer Base Address are
   // always zero and not programmable to allow for 128 byte alignment
   // and cache line write optimizations."
   //So we don't need to do any shift to know the address
   //The & with 0xFFFFFFFE is done to avoid the Buffer position
   //enable bit to be considered part of the address in use.
   Print(L"   DMA Position Lower Base Address (DPLBASE): 0x%x\r\n",
        ControllerRegisterSet->DPIBLBASE & 0xFFFFFFFE);

   Print(L"DMA Position Upper Base Address (DPIBUBASE): 0x%x\r\n\n",
     ControllerRegisterSet->DPIBUBASE);
*/
/*

   Print(L"Immediate Command Output Interface (ICOI): 0x%x\r\n",
     ControllerRegisterSet->ICOI);
   Print(L"   Immediate Command Write (ICW): 0x%x\r\n",
        ControllerRegisterSet->ICOI);


   Print(L"Immediate Response Input Interface (ICII): 0x%x\r\n",
     ControllerRegisterSet->ICII);
   Print(L"   Immediate Response Read (IRR): 0x%x\r\n",
        ControllerRegisterSet->ICII);


   Print(L"Immediate Command Status (ICIS): 0x%x\r\n",
     ControllerRegisterSet->ICIS);
   Print(L"   Immediate Command Busy (ICB): %d\r\n",
     ControllerRegisterSet->ICIS & 0x1);
   Print(L"   Immediate Result Valid (IRV): %d\r\n",
       (ControllerRegisterSet->ICIS >> 1) & 0x1);
   Print(L"   Immediate Command Version: %d\r\n",
       (ControllerRegisterSet->ICIS >> 2) & 0x1);
   Print(L"   Immediate Response Result Unsolicited (IRRUNSOL): 0x%x\r\n",
       (ControllerRegisterSet->ICIS >> 3) & 0x1);
   Print(L"   Immediate Response Result Address (IRRADD): 0x%x\r\n\n",
       (ControllerRegisterSet->ICIS >> 4) & 0xF);


   Print(L"--------Input Stream Descriptors--------\r\n");
   for(Count = 0; Count < IssCount; Count++) {
    Print(L"Input Stream %d: \r\n", Count);
    ParseStreamDescriptor(ControllerRegisterSet->ISS[Count]);
   }
   Print(L"----------------------------------------\r\n");
*/

   Print(L"--------Output Stream Descriptors--------\r\n");
   //for(Count = 0; Count < OssCount; Count++) {
   for(Count = 0; Count < 1; Count++) {
    Print(L"Output Stream %d: \r\n", Count);
    ParseStreamDescriptor(ControllerRegisterSet->OSS[Count]);
   }
   Print(L"----------------------------------------\r\n");

/*
   Print(L"--------Bidirectional Stream Descriptors--------\r\n");
   for(Count = 0; Count < BssCount; Count++) {
    Print(L"Bidirectional Stream %d: \r\n", Count);
    ParseStreamDescriptor(ControllerRegisterSet->BSS[Count]);
   }
   Print(L"----------------------------------------\r\n");

   Print(L"\r\n***********************************\r\n");
*/
 } else {
  Status = EFI_INVALID_PARAMETER;
 }

 return Status;
}

CHAR16* GetNodeType(UINT32 WidgetCap) {

 switch (HDA_WIDGET_TYPE(WidgetCap)) {
   case HDA_WIDGET_TYPE_AUDIO_OUTPUT:
    return L"Audio Output";
    break;
   case HDA_WIDGET_TYPE_AUDIO_INPUT:
    return L"Audio Input";
    break;
   case HDA_WIDGET_TYPE_AUDIO_MIXER:
    return L" Mixer";
    break;
   case HDA_WIDGET_TYPE_AUDIO_SELECTOR:
    return L"Audio Selector";
    break;
   case HDA_WIDGET_TYPE_AUDIO_PIN_CONPLEX:
    return L"Pin Complex";
    break;
   case HDA_WIDGET_TYPE_AUDIO_POWER:
    return L"Audio Power";
    break;
   case HDA_WIDGET_TYPE_AUDIO_VOLUME_KNOB:
    return L"Volume Knob";
    break;
   case HDA_WIDGET_TYPE_AUDIO_BEEP_GENERATOR:
    return L"Beep Generator";
    break;
   case HDA_WIDGET_TYPE_AUDIO_VENDOR_DEFINED:
    return L"Vendor Defined";
    break;
   default:
    return L"Unknown";
    break;
  }
}

VOID PrintNodeData(CHAR16* Identation, struct Node *CurrentNode)
{

	 //this value represent the maximum number of channels accepted
	 //at a given widget
	 UINT32 ChanCount = 0;

	 Print(L"%sNID: 0x%x\r\n", Identation, CurrentNode->NodeId);

	 if(CurrentNode->NodeType == HDA_NODE_ROOT) {

	  Print(L"%sVendor: 0x%x\r\n", Identation, CurrentNode->VendorId);
		 Print(L"%sRevision: 0x%x\r\n",Identation, CurrentNode->RevisionId);
	 }


	 if(CurrentNode->NodeType == HDA_NODE_ROOT ||
	  CurrentNode->NodeType == HDA_NODE_FUNCTION_GROUP) {

	  Print(L"%sTotal Node Count: 0x%x\r\n", Identation,
		  CurrentNode->SubordinateNodeCount);
	  Print(L"%sStart Node Count: 0x%x\r\n", Identation,
			 CurrentNode->StartingChildNodeAddess);

	  if(CurrentNode->NodeType == HDA_NODE_ROOT) {
	   goto FINISH;
	  }
	 }


	if(CurrentNode->NodeType == HDA_NODE_FUNCTION_GROUP) {

		Print(L"%sFunction Group Node Type: 0x%x\r\n",
					Identation, CurrentNode->FunctionGroupType);

		switch (CurrentNode->FunctionGroupType & 0xFF) {
		 case HDA_FUNCTION_GROUP_TYPE_AUDIO:
		  Print(L"%s   Node Type: Audio Function Group\r\n",Identation);
		  break;
		 case HDA_FUNCTION_GROUP_TYPE_MODEM:
		   Print(L"%s   Node Type: Modem Function Group\r\n",Identation);
		   break;
		 default:
		  Print(L"%s   Node Type: Reserved or Vendor defined\r\n",Identation);
		  break;
		}

		Print(L"%s   Unsol Capable: 0x%x\r\n",Identation,
		(CurrentNode->FunctionGroupType >> 8) & 0x1);
	}

	if(CurrentNode->NodeType == HDA_NODE_FUNCTION_GROUP) {

	  switch (CurrentNode->FunctionGroupType & 0xFF) {
		 case HDA_FUNCTION_GROUP_TYPE_AUDIO:
		Print(L"%sAudio Function Group Capabilities: 0x%x\r\n",
		  Identation, CurrentNode->FuncCap);
		Print(L"%s   Output Delay: 0x%x\r\n",
		   Identation, CurrentNode->FuncCap & 0x7);
		Print(L"%s   Input Delay: 0x%x\r\n",
		   Identation, (CurrentNode->FuncCap >> 8) & 0x7);
		Print(L"%s   Beep Generator: 0x%x\r\n",
		   Identation, (CurrentNode->FuncCap >> 16) & 0x1);

		break;
	  }
	}

	//in case it's any kind of widget
	if(CurrentNode->NodeType != HDA_NODE_ROOT &&
	  CurrentNode->NodeType != HDA_NODE_FUNCTION_GROUP)
	{
	  ChanCount = (CurrentNode->WidgetCap & 0x1) +
			((CurrentNode->WidgetCap >> 13) & 0x7);

	  Print(L"%sAudio Widget Capabilities: 0x%x\r\n",
		Identation, CurrentNode->WidgetCap);
	  Print(L"%s   Maximum Channel Count: 0x%x\r\n",
		 Identation, ChanCount);
	  Print(L"%s   Input Amplifier Present: %d\r\n",
		 Identation, (CurrentNode->WidgetCap >> 1) & 0x1);
	  Print(L"%s   Output Amplifier Present: %d\r\n",
		 Identation, (CurrentNode->WidgetCap >> 2) & 0x1);
	  Print(L"%s   Amplifier Parameter Override: %d\r\n",
		 Identation, (CurrentNode->WidgetCap >> 3) & 0x1);
	  Print(L"%s   Format Override: %d\r\n",
		 Identation, (CurrentNode->WidgetCap >> 4) & 0x1);
	  Print(L"%s   Stripe Supported: %d\r\n",
		 Identation, (CurrentNode->WidgetCap >> 5) & 0x1);
	  Print(L"%s   ProcWidget (in case yes check the 'Processing Controls' parameter): %d\r\n",
		 Identation, (CurrentNode->WidgetCap >> 6) & 0x1);
	  Print(L"%s   Unsol Capable (Supports unsolicited responses): %d\r\n",
		 Identation, (CurrentNode->WidgetCap >> 7) & 0x1);
	  Print(L"%s   Connection List Present: %d\r\n",
		 Identation, (CurrentNode->WidgetCap >> 8) & 0x1);
	  Print(L"%s   Digital (Supports Digital Streams): %d\r\n",
		 Identation, (CurrentNode->WidgetCap >> 9) & 0x1);
	  Print(L"%s   Power State Control: %d\r\n",
		 Identation, (CurrentNode->WidgetCap >> 10) & 0x1);
	  Print(L"%s   L-R Swap: %d\r\n",
		 Identation, (CurrentNode->WidgetCap >> 11) & 0x1);
	  Print(L"%s   Content Protection: %d\r\n",
		 Identation, (CurrentNode->WidgetCap >> 12) & 0x1);
	  Print(L"%s   Delay (Number of Sample Delays): %d\r\n",
		 Identation, (CurrentNode->WidgetCap >> 16) & 0xF);

	  Print(L"%s   Type: %s\r\n", Identation,
		GetNodeType(CurrentNode->WidgetCap));
	}


    if(CurrentNode->WidgetType == HDA_WIDGET_TYPE_AUDIO_OUTPUT ||
     CurrentNode->WidgetType == HDA_WIDGET_TYPE_AUDIO_INPUT ||
     CurrentNode->WidgetType == HDA_WIDGET_TYPE_AUDIO_VENDOR_DEFINED ||
     ((CurrentNode->FunctionGroupType & 0xFF) == HDA_FUNCTION_GROUP_TYPE_AUDIO)) {

	  Print(L"%sSample Size and Rate Capabilities: 0x%x\r\n",
		Identation, CurrentNode->SampleSizeRateCap);
	  Print(L"%s   8 kHz support: %d\r\n",
		Identation, CurrentNode->SampleSizeRateCap & 1);
	  Print(L"%s   11.025 kHz support: %d\r\n",
		Identation, (CurrentNode->SampleSizeRateCap >> 1) & 0x1);
	  Print(L"%s   16.0 kHz support: %d\r\n",
		Identation, (CurrentNode->SampleSizeRateCap >> 2) & 0x1);
	  Print(L"%s   22.05 kHz support: %d\r\n",
		Identation, (CurrentNode->SampleSizeRateCap >> 3) & 0x1);
	  Print(L"%s   32.0 kHz support: %d\r\n",
		Identation, (CurrentNode->SampleSizeRateCap >> 4) & 0x1);
	  Print(L"%s   44.1 kHz support: %d\r\n",
		Identation, (CurrentNode->SampleSizeRateCap >> 5) & 0x1);
	  Print(L"%s   48.0 kHz support: %d\r\n",
		Identation, (CurrentNode->SampleSizeRateCap >> 6) & 0x1);
	  Print(L"%s   88.2 kHz support: %d\r\n",
		Identation, (CurrentNode->SampleSizeRateCap >> 7) & 0x1);
	  Print(L"%s   96.0 kHz support: %d\r\n",
		Identation, (CurrentNode->SampleSizeRateCap >> 8) & 0x1);
	  Print(L"%s   176.4 kHz support: %d\r\n",
		Identation, (CurrentNode->SampleSizeRateCap >> 9) & 0x1);
	  Print(L"%s   192.0 kHz support: %d\r\n",
		Identation, (CurrentNode->SampleSizeRateCap >> 10) & 0x1);
	  Print(L"%s   384 kHz support: %d\r\n",
		Identation, (CurrentNode->SampleSizeRateCap >> 11) & 0x1);
	  Print(L"%s   8 bit audio format supported : %d\r\n",
		Identation, (CurrentNode->SampleSizeRateCap >> 16) & 0x1);
	  Print(L"%s   16 bit audio format supported : %d\r\n",
		Identation, (CurrentNode->SampleSizeRateCap >> 17) & 0x1);
	  Print(L"%s   20 bit audio format supported : %d\r\n",
		Identation, (CurrentNode->SampleSizeRateCap >> 18) & 0x1);
	  Print(L"%s   24 bit audio format supported : %d\r\n",
		Identation, (CurrentNode->SampleSizeRateCap >> 19) & 0x1);
	  Print(L"%s   32 bit audio format supported : %d\r\n",
		Identation, (CurrentNode->SampleSizeRateCap >> 20) & 0x1);

	  Print(L"%sStream Formats: 0x%x\r\n",
		Identation, CurrentNode->StreamFormat);
	  Print(L"%s   PCM supported: 0x%x\r\n",
		Identation, CurrentNode->StreamFormat & 0x1);
	  Print(L"%s   Float32 supported: 0x%x\r\n",
		Identation, (CurrentNode->StreamFormat >> 1) & 0x1);
	  Print(L"%s   AC3 supported: 0x%x\r\n",
		Identation, (CurrentNode->StreamFormat >> 2) & 0x1);
    }


    if(CurrentNode->WidgetType == HDA_WIDGET_TYPE_AUDIO_PIN_CONPLEX ||
     CurrentNode->WidgetType == HDA_WIDGET_TYPE_AUDIO_VENDOR_DEFINED) {

	  Print(L"%sPin Capabilities: 0x%x\r\n",
		Identation, CurrentNode->PinCap);
	  Print(L"%s   Impedance Sense Capable: %d\r\n",
		Identation, CurrentNode->PinCap & 1);
	  Print(L"%s   Trigger Required: %d\r\n",
		 Identation, (CurrentNode->PinCap >> 1) & 0x1);
	  Print(L"%s   Presence Detect Capable: %d\r\n",
		 Identation, (CurrentNode->PinCap >> 2) & 0x1);
	  Print(L"%s   Head-phone Drive Capable: %d\r\n",
		 Identation, (CurrentNode->PinCap >> 3) & 0x1);
	  Print(L"%s   Output Capable: %d\r\n",
		 Identation, (CurrentNode->PinCap >> 4) & 0x1);
	  Print(L"%s   Input Capable: %d\r\n",
		 Identation, (CurrentNode->PinCap >> 5) & 0x1);
	  Print(L"%s   Balanced I/O Pins: %d\r\n",
		 Identation, (CurrentNode->PinCap >> 6) & 0x1);
	  Print(L"%s   HDMI: %d\r\n",
		 Identation, (CurrentNode->PinCap >> 7) & 0x1);

	  Print(L"%s   VRef Control: %d\r\n",
		 Identation, (CurrentNode->PinCap >> 8) & 0xFF);
	  Print(L"%s      Hi-Z: %d\r\n",
		 Identation, (CurrentNode->PinCap >> 8) & 0x1);
	  Print(L"%s      50%: %d\r\n",
		 Identation, (CurrentNode->PinCap >> 9) & 0x1);
	  Print(L"%s      Ground: %d\r\n",
		 Identation, (CurrentNode->PinCap >> 10) & 0x1);
	  Print(L"%s      80%: %d\r\n",
		 Identation, (CurrentNode->PinCap >> 12) & 0x1);
	  Print(L"%s      100%: %d\r\n",
		 Identation, (CurrentNode->PinCap >> 13) & 0x1);

	  Print(L"%s   EAPD Capable: %d\r\n",
		 Identation, (CurrentNode->PinCap >> 16) & 0x1);
	  Print(L"%s   DP (Display Port): %d\r\n",
		 Identation, (CurrentNode->PinCap >> 24) & 0x1);
	  Print(L"%s   HBR (High Bit Rate): %d\r\n",
		 Identation, (CurrentNode->PinCap >> 27) & 0x1);
    }


    if(CurrentNode->WidgetType == HDA_WIDGET_TYPE_AUDIO_INPUT ||
  CurrentNode->WidgetType == HDA_WIDGET_TYPE_AUDIO_PIN_CONPLEX ||
  CurrentNode->WidgetType == HDA_WIDGET_TYPE_AUDIO_MIXER ||
  CurrentNode->WidgetType == HDA_WIDGET_TYPE_AUDIO_SELECTOR ||
  CurrentNode->WidgetType == HDA_WIDGET_TYPE_AUDIO_VENDOR_DEFINED ||
  ((CurrentNode->FunctionGroupType & 0xFF) == HDA_FUNCTION_GROUP_TYPE_AUDIO)) {

	  Print(L"%sInput Amp Capabilities: 0x%x\r\n",
		Identation, CurrentNode->InputAmpCap);
	  Print(L"%s   Offset: %d (representing 0 dB)\r\n",
		Identation, CurrentNode->InputAmpCap & 0x3F);
	  Print(L"%s   Number of Steps: %d\r\n",
		Identation, (CurrentNode->InputAmpCap >> 8) & 0x7F);
	  Print(L"%s   Step Size: %d\r\n",
		Identation, (CurrentNode->InputAmpCap >> 16) & 0x7F);
	  Print(L"%s   Mute Capable: %d\r\n",
		Identation, (CurrentNode->InputAmpCap >> 31) & 0x1);
    }


    if(CurrentNode->WidgetType == HDA_WIDGET_TYPE_AUDIO_OUTPUT ||
  CurrentNode->WidgetType == HDA_WIDGET_TYPE_AUDIO_PIN_CONPLEX ||
  CurrentNode->WidgetType == HDA_WIDGET_TYPE_AUDIO_MIXER ||
  CurrentNode->WidgetType == HDA_WIDGET_TYPE_AUDIO_SELECTOR ||
  CurrentNode->WidgetType == HDA_WIDGET_TYPE_AUDIO_VENDOR_DEFINED ||
  ((CurrentNode->FunctionGroupType & 0xFF) == HDA_FUNCTION_GROUP_TYPE_AUDIO)) {

	  Print(L"%sOutput Amp Capabilities: 0x%x\r\n",
		Identation, CurrentNode->OutputAmpCap);
	  Print(L"%s   Offset: %d\r\n",
		 Identation, CurrentNode->OutputAmpCap & 0x3F);
	  Print(L"%s   Number of Steps: %d\r\n",
		Identation, (CurrentNode->OutputAmpCap >> 8) & 0x7F);
	  Print(L"%s   Step Size: %d\r\n",
		Identation, (CurrentNode->OutputAmpCap >> 16) & 0x7F);
	  Print(L"%s   Mute Capable: %d\r\n",
		Identation, (CurrentNode->OutputAmpCap >> 31) & 0x1);
    }


    if(CurrentNode->WidgetType == HDA_WIDGET_TYPE_AUDIO_INPUT ||
      CurrentNode->WidgetType == HDA_WIDGET_TYPE_AUDIO_PIN_CONPLEX ||
      CurrentNode->WidgetType == HDA_WIDGET_TYPE_AUDIO_MIXER ||
      CurrentNode->WidgetType == HDA_WIDGET_TYPE_AUDIO_SELECTOR ||
      CurrentNode->WidgetType == HDA_WIDGET_TYPE_AUDIO_POWER ||
      CurrentNode->WidgetType == HDA_WIDGET_TYPE_AUDIO_VOLUME_KNOB ||
      CurrentNode->WidgetType == HDA_WIDGET_TYPE_AUDIO_VENDOR_DEFINED) {

	  Print(L"%sConnection List Length: 0x%x\r\n",
		Identation, CurrentNode->ConnectionListLength);
	  Print(L"%s   Connection List Length: %d\r\n",
		Identation, CurrentNode->ConnectionListLength & 0x3F);
	  Print(L"%s   Long Form: %d\r\n",
		Identation, (CurrentNode->ConnectionListLength >> 7) & 0x1);
    }

    if(CurrentNode->NodeType != HDA_NODE_ROOT) {

	  Print(L"%sSupported Power States: 0x%x\r\n",
		Identation, CurrentNode->SupportedPowerStates);
	  Print(L"%s  D0 Supported: %d\r\n",
		Identation, CurrentNode->SupportedPowerStates & 0x1);
	  Print(L"%s  D1 Supported: %d\r\n",
		Identation, (CurrentNode->SupportedPowerStates >> 1) & 0x1);
	  Print(L"%s  D2 Supported: %d\r\n",
		Identation, (CurrentNode->SupportedPowerStates >> 2) & 0x1);
	  Print(L"%s  D3 Supported: %d\r\n",
		Identation, (CurrentNode->SupportedPowerStates >> 3) & 0x1);
	  Print(L"%s  D3 Cold Supported: %d\r\n",
		Identation, (CurrentNode->SupportedPowerStates >> 4) & 0x1);
	  Print(L"%s  S3 D3 Cold Supported: %d\r\n",
		Identation, (CurrentNode->SupportedPowerStates >> 29) & 0x1);
	  Print(L"%s  CLKSTOP (Clock Stop): %d\r\n",
		Identation, (CurrentNode->SupportedPowerStates >> 30) & 0x1);
	  Print(L"%s  EPSS Capabilities Supported: %d\r\n",
		Identation, (CurrentNode->SupportedPowerStates >> 31) & 0x1);
    }

    if(CurrentNode->WidgetType == HDA_WIDGET_TYPE_AUDIO_OUTPUT ||
  CurrentNode->WidgetType == HDA_WIDGET_TYPE_AUDIO_INPUT ||
  CurrentNode->WidgetType == HDA_WIDGET_TYPE_AUDIO_PIN_CONPLEX ||
  CurrentNode->WidgetType == HDA_WIDGET_TYPE_AUDIO_SELECTOR) {

	  Print(L"%sProcessing Capabilities: 0x%x\r\n",
		Identation, CurrentNode->ProcessingCap);
	  Print(L"%s   Benign: %d\r\n",
		Identation, CurrentNode->ProcessingCap & 0x1);
	  Print(L"%s   Number of Coefficients (NumCoeff): %d\r\n",
		Identation, (CurrentNode->ProcessingCap >> 8) & 0xFF);
    }


    if(CurrentNode->NodeType != HDA_NODE_FUNCTION_GROUP &&
     CurrentNode->NodeType != HDA_NODE_ROOT) {

	  Print(L"%sGP I/O Count: 0x%x\r\n",
		Identation, CurrentNode->GPIOCount);
	  Print(L"%s   Number of GPIOs: 0x%x\r\n",
		Identation, CurrentNode->GPIOCount & 0xFF);
	  Print(L"%s   Number of GPOs: 0x%x\r\n",
		Identation, (CurrentNode->GPIOCount >> 8) & 0xFF);
	  Print(L"%s   Number of GPIs: 0x%x\r\n",
		Identation, (CurrentNode->GPIOCount >> 16) & 0xFF);
	  Print(L"%s   GPIO Unsolicited: 0x%x\r\n",
		Identation, (CurrentNode->GPIOCount >> 30) & 0x1);
	  Print(L"%s   GPIO Wake: 0x%x\r\n",
		Identation, (CurrentNode->GPIOCount >> 31) & 0x1);
    }

    if(CurrentNode->WidgetType == HDA_WIDGET_TYPE_AUDIO_VOLUME_KNOB) {

	  Print(L"%sVolume Knob Capabilities: 0x%x\r\n",
		Identation, CurrentNode->VolKnobCap);
	  Print(L"%s   Number of Steps: %d\r\n",
		Identation, CurrentNode->VolKnobCap & 0x3F);
	  Print(L"%s   Delta: %d\r\n",
		Identation, (CurrentNode->VolKnobCap >> 7) & 0x1);
    }


    if(CurrentNode->NodeType != HDA_NODE_ROOT) {
		Print(L"%sPower State: 0x%x\r\n",
				Identation, CurrentNode->PowerState);

		Print(L"%s   PS-Set: %d\r\n",
				Identation, CurrentNode->PowerState & 0xF);
		Print(L"%s   PS-Act: %d\r\n",
				Identation, (CurrentNode->PowerState >> 4) & 0xF);
		Print(L"%s   PS-Error: %d\r\n",
				Identation, (CurrentNode->PowerState >> 8) & 0x1);
		Print(L"%s   PS-ClkStopOk: %d\r\n",
				Identation, (CurrentNode->PowerState >> 9) & 0x1);
		Print(L"%s   PS-SettingsReset: %d\r\n",
				Identation, (CurrentNode->PowerState >> 10) & 0x1);
    }

    if(CurrentNode->WidgetType == HDA_WIDGET_TYPE_AUDIO_OUTPUT ||
      CurrentNode->WidgetType == HDA_WIDGET_TYPE_AUDIO_INPUT){
    	Print(L"%sChannelId: 0x%x\r\n",
    	    	Identation, CurrentNode->ChannelStreamId & 0xF);
    	Print(L"%sStreamId: 0x%x\r\n",
    			Identation, (CurrentNode->ChannelStreamId >> 4) & 0xF);
    }

  if(CurrentNode->WidgetType == HDA_WIDGET_TYPE_AUDIO_OUTPUT ||
  CurrentNode->WidgetType == HDA_WIDGET_TYPE_AUDIO_INPUT ||
  CurrentNode->WidgetType == HDA_WIDGET_TYPE_AUDIO_PIN_CONPLEX ||
  CurrentNode->WidgetType == HDA_WIDGET_TYPE_AUDIO_MIXER ||
  CurrentNode->WidgetType == HDA_WIDGET_TYPE_AUDIO_SELECTOR ||
  CurrentNode->WidgetType == HDA_WIDGET_TYPE_AUDIO_BEEP_GENERATOR ||
  CurrentNode->WidgetType == HDA_WIDGET_TYPE_AUDIO_VENDOR_DEFINED ) {

		Print(L"%sRight Channel Gain/Mute: 0x%x\r\n",
				Identation, CurrentNode->RightGain);

		Print(L"%s   Amplifier Gain: %d\r\n",
				Identation, CurrentNode->RightGain & 0xF);
		Print(L"%s   Amplifier Mute: %d\r\n",
				Identation, (CurrentNode->RightGain >> 7) & 0x1);

		Print(L"%sLeft Channel Gain/Mute: 0x%x\r\n",
				Identation, CurrentNode->LeftGain);

		Print(L"%s   Amplifier Gain: %d\r\n",
				Identation, CurrentNode->LeftGain & 0xF);
		Print(L"%s   Amplifier Mute: %d\r\n",
				Identation, (CurrentNode->LeftGain >> 7) & 0x1);

    }

FINISH:

    Print(L"\n");
 return;
}


EFI_STATUS ParseCodecData (struct Node *RootNode) {

 struct Node *CurrentFunctionGroupNode = NULL;
 struct Node *CurrentWidgetNode = NULL;

 UINT32 CurrentFunctionGroupId = 0;
 UINT32 CurrentFgArrayIndexer = 0;

 UINT32 CurrentWidgetId = 0;
 UINT32 CurrentWidgetArrayIndexer = 0;

 Print(L"\r\n****HDA CODEC Information****\r\n");

 PrintNodeData(L" ", RootNode);

 for(CurrentFunctionGroupId = RootNode->StartingChildNodeAddess;
  CurrentFunctionGroupId < RootNode->StartingChildNodeAddess +
  RootNode->SubordinateNodeCount; CurrentFunctionGroupId++,
  CurrentFgArrayIndexer++) {

  //Fills function group information
  CurrentFunctionGroupNode = &(RootNode->ChildNodes[CurrentFgArrayIndexer]);
  PrintNodeData(L"    ", CurrentFunctionGroupNode);

  CurrentWidgetArrayIndexer = 0;

  for(CurrentWidgetId = CurrentFunctionGroupNode->StartingChildNodeAddess;
   CurrentWidgetId < CurrentFunctionGroupNode->StartingChildNodeAddess +
   CurrentFunctionGroupNode->SubordinateNodeCount;
   CurrentWidgetId++, CurrentWidgetArrayIndexer++) {

   //fills widget information
   CurrentWidgetNode = &(CurrentFunctionGroupNode->ChildNodes[CurrentWidgetArrayIndexer]);
   PrintNodeData(L"       ", CurrentWidgetNode);

  }
 }

 return EFI_SUCCESS;
}

EFI_STATUS
ParseCodecConectionList (
		PCI_HDA_REGION* PcieDeviceConfigSpace,
        struct Node *RootNode)
{

 struct Node *CurrentFunctionGroupNode = NULL;
 struct Node *CurrentWidgetNode = NULL;

 struct Node SearchNode;

 UINT32 CurrentFunctionGroupId = 0;
 UINT32 CurrentFgArrayIndexer = 0;

 UINT32 CurrentWidgetId = 0;
 UINT32 CurrentWidgetArrayIndexer = 0;

 UINT32 VerbResponse = 0;
 UINT32 NodeConnected1 = 0;
 UINT32 NodeConnected2 = 0;
 UINT32 NodeConnected3 = 0;
 UINT32 NodeConnected4 = 0;

 Print(L"\r\n****HDA CODEC Connections****\r\n\n");

 for(CurrentFunctionGroupId = RootNode->StartingChildNodeAddess;
  CurrentFunctionGroupId < RootNode->StartingChildNodeAddess +
  RootNode->SubordinateNodeCount; CurrentFunctionGroupId++,
  CurrentFgArrayIndexer++) {

  //Fills function group information
  CurrentFunctionGroupNode = &(RootNode->ChildNodes[CurrentFgArrayIndexer]);
  CurrentWidgetArrayIndexer = 0;

  for(CurrentWidgetId = CurrentFunctionGroupNode->StartingChildNodeAddess;
   CurrentWidgetId < CurrentFunctionGroupNode->StartingChildNodeAddess +
   CurrentFunctionGroupNode->SubordinateNodeCount;
   CurrentWidgetId++, CurrentWidgetArrayIndexer++) {

   //fills widget information
   CurrentWidgetNode = &(CurrentFunctionGroupNode->ChildNodes[CurrentWidgetArrayIndexer]);

   Print(L"Node ID: 0x%x (%s), Connections: %d\r\n",
     CurrentWidgetNode->NodeId,
     GetNodeType(CurrentWidgetNode->WidgetCap),
     	 	 	 CurrentWidgetNode->ConnectionListLength);

   if(CurrentWidgetNode->ConnectionListLength > 0) {
	   GetCodecData8BitPayloadCorbRirb(PcieDeviceConfigSpace,0,
									   CurrentWidgetNode->NodeId,
									   HDA_VRB_GET_GET_CONNECTION_LIST_ENTRY,
									   HDA_VRB_EMPTY_PAYLOAD, &VerbResponse);

    NodeConnected1 = VerbResponse & 0xFF;
    NodeConnected2 = (VerbResponse >> 8) & 0xFF;
    NodeConnected3 = (VerbResponse >> 16) & 0xFF;
    NodeConnected4 = (VerbResponse >> 24) & 0xFF;

    Print(L"   0x%x(%s)",CurrentWidgetNode->NodeId,
    		GetNodeType(CurrentWidgetNode->WidgetCap));

    if(NodeConnected1 > 0) {
     GetNodeById(RootNode, NodeConnected1, &SearchNode);
     Print(L"--> 0x%x(%s)", NodeConnected1,
    		 GetNodeType(SearchNode.WidgetCap));
    }
    if(NodeConnected2 > 0) {
     GetNodeById(RootNode, NodeConnected2, &SearchNode);
     Print(L"--> 0x%x(%s)", NodeConnected2,
    		 GetNodeType(SearchNode.WidgetCap));
    }
    if(NodeConnected3 > 0) {
     GetNodeById(RootNode, NodeConnected3, &SearchNode);
     Print(L"--> 0x%x(%s)", NodeConnected3,
    		 GetNodeType(SearchNode.WidgetCap));
    }
    if(NodeConnected4 > 0) {
     GetNodeById(RootNode, NodeConnected4, &SearchNode);
     Print(L"--> 0x%x(%s)", NodeConnected4,
    		 GetNodeType(SearchNode.WidgetCap));
    }
    Print(L"\r\n");

    GetCodecData8BitPayloadCorbRirb(PcieDeviceConfigSpace,0,
									CurrentWidgetNode->NodeId,
									HDA_VRB_GET_CONNECTION_SELECT,
									HDA_VRB_EMPTY_PAYLOAD, &VerbResponse);

    Print(L"   Active connection: 0x%x\r\n",
    		VerbResponse & 0xFF);

   }
   Print(L"\r\n");
  }
 }


 return EFI_SUCCESS;
}
