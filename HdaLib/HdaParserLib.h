/** @file

    Copyright (c) 2018, Rafael Rodrigues Machado. All rights reserved.<BR>
    This program and the accompanying materials
    are licensed and made available under the terms and conditions of the BSD License
    which accompanies this distribution. The full text of the license may be found at
    http://opensource.org/licenses/bsd-license.

    THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
    WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

#ifndef _HDA_PARSER_LIB_H
#define _HDA_PARSER_LIB_H


EFI_STATUS ParsePcieDeviceData (PCI_HDA_REGION* PcieDeviceConfigSpace);


VOID ParseStreamDescriptor (HDA_CONTROLLER_STREAM_DESCRIPTOR StreamDescriptor);


EFI_STATUS ParseControllerRegisterSet (HDA_CONTROLLER_REGISTER_SET* ControllerRegisterSet);

CHAR16* GetNodeType(UINT32 WidgetCap);

VOID PrintNodeData(CHAR16* Identation, struct Node *CurrentNode);


EFI_STATUS ParseCodecData (struct Node *RootNode);

EFI_STATUS ParseCodecConectionList (PCI_HDA_REGION* PcieDeviceConfigSpace,
                   struct Node *RootNode);
#endif
