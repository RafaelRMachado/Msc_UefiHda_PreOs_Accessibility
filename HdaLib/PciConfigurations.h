/** @file

    Copyright (c) 2018, Rafael Rodrigues Machado. All rights reserved.<BR>
    This program and the accompanying materials
    are licensed and made available under the terms and conditions of the BSD License
    which accompanies this distribution. The full text of the license may be found at
    http://opensource.org/licenses/bsd-license.

    THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
    WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

#ifndef HDALIB_PCICONFIGURATIONS_H_
#define HDALIB_PCICONFIGURATIONS_H_

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/PciIo.h>

#include "HdaLib.h"

EFI_STATUS TearDownCommonBuffer (
  UINT8 *HostAddress,
  UINTN Length,
  VOID *Mapping);

EFI_STATUS ReleaseHdaPciIoHandler ();

EFI_STATUS SetupCommonBuffer (
	UINT8 **HostAddress,
	UINTN Length,
	VOID **Mapping,
	UINTN Alignment);

EFI_STATUS GetHdaPciIoHandler ();

EFI_STATUS GetRootBridgeIoHandler ();

EFI_STATUS GetControllerRegisterSet (
	PCI_HDA_REGION* PcieDeviceConfigSpace,
	HDA_CONTROLLER_REGISTER_SET* ControllerRegisterSet);


EFI_STATUS WritePciMemory (
	UINT64 Address,
	VOID* Value,
	UINTN Count,
	EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH Width);

EFI_STATUS ReadPciMemory (
	UINT64 Address,
	VOID* Value,
	UINTN Count,
	EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH Width);

EFI_STATUS WriteControllerRegister (
	PCI_HDA_REGION* PcieDeviceConfigSpace,
	UINT64 Offset,
	VOID* Value,
	UINTN Count,
	UINT8 BarIndex,
	EFI_PCI_IO_PROTOCOL_WIDTH Width);

EFI_STATUS ReadControllerRegister (
	PCI_HDA_REGION* PcieDeviceConfigSpace,
	UINT64 Offset,
	VOID* Value,
	UINTN Count,
	EFI_PCI_IO_PROTOCOL_WIDTH Width);

EFI_STATUS ReadControllerPcieConfiguration (
	PCI_HDA_REGION* PcieDeviceConfigSpace,
	UINT64 Offset,
	VOID* Value,
	UINTN Count,
	EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH Width);

EFI_STATUS WriteControllerPcieConfiguration (
	PCI_HDA_REGION* PcieDeviceConfigSpace,
	UINT64 Offset,
	VOID* Value,
	UINTN Count,
	EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH Width);

EFI_STATUS GetPcieConfigSpace (
	UINTN Bus,
	UINTN Device,
	UINTN Function,
	PCI_HDA_REGION* PcieDeviceConfigSpace );

#endif
