/** @file

    Copyright (c) 2018, Rafael Rodrigues Machado. All rights reserved.<BR>
    This program and the accompanying materials
    are licensed and made available under the terms and conditions of the BSD License
    which accompanies this distribution. The full text of the license may be found at
    http://opensource.org/licenses/bsd-license.

    THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
    WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

#ifndef _HDA_LIB_H
#define _HDA_LIB_H

#include<Uefi.h>
#include <IndustryStandard/Pci.h>
#include <Protocol/PciIo.h>

//#include "PciConfigurations.h"

/** @file
  This Library contains definitions and functions
  needed to work with Intel High Definition compatible
  controllers and codecs.
**/


/* Location of the Intel HDA Controller defined at
 * intel's chipset manuals
 */
#define HDA_BUS 0
#define HDA_DEV 27
#define HDA_FUNC 0


/*
 * As described at HDA Spec, page 57:
 * The Buffer Descriptor List (BDL) is a memory structure that describes the buffers in memory. The
 * BDL is comprised of a series of Buffer Descriptor List Entries. There must be at least two entries
 * in the list, with a maximum of 256 entries. Also, the start of the structure must be aligned on a 128
*/
#define HDA_BUFFER_DESC_LIST_MAX_ENTRIES 256


///
/// HD header defined at the IO-Controller_Hub-7-hd-audio-ac97 manual
/// As described at page 13 of the documentation, the addresses not defined
/// should be treated as reserved
///
typedef struct {
	UINT16	VID;
	UINT16	DID;
	UINT16	PCICMD;
	UINT16	PCISTS;
	UINT8	RID;
	UINT8	PI;
	UINT8	SC;
	UINT8	BCC;
	UINT8	CLS;
	UINT8	LT;
	UINT8	HEADTYP;
	UINT8   RES0;
	UINT32	HDBARL;
	UINT32	HDBARU;
	UINT8   RESV1[20];
	UINT16	SVID;
	UINT16	SID;
	UINT8   RESV2[4];
	UINT8	CAPPTR;
	UINT8   RESV3[7];
	UINT8	INTLN;
	UINT8	INTPN;
	UINT8   RESV4[2];
	UINT8	HDCTL;
	UINT8   RESV5[3];
	UINT8	TCSEL;
	UINT8   RESV6[8];
	UINT8	DCKSTS;
	UINT8   RESV7[2];
	UINT16	PID;
	UINT16	PC;
	UINT32	PCS;
	UINT8   RESV8[8];
	UINT16	MID;
	UINT16	MMC;
	UINT32	MMLA;
	UINT32	MMUA;
	UINT16	MMD;
	UINT8   RESV9[2];
	UINT16	PXID;
	UINT16	PXC;
	UINT32	DEVCAP;
	UINT16	DEVCTL;
	UINT16	DEVS;
	UINT8   RESV10[132];
	UINT32	VCCAP;
	UINT32	PVCCAP1;
	UINT32	PVCCAP2;
	UINT16	PVCCTL;
	UINT16	PVCSTS;
	UINT32	VC0CAP;
	UINT32	VC0CTL;
	UINT8   RESV11[2];
	UINT16	VC0STS;
	UINT32	VciCAP;
	UINT32	VciCTL;
	UINT8   RESV12[2];
	UINT16	VciSTS;
	UINT8   RESV13[8];
	UINT32	RCCAP;
	UINT32	ESD;
	UINT8   RESV14[8];
	UINT32	L1DESC;
	UINT8   RESV15[4];
	UINT32	L1ADDL;
	UINT32	L1ADDU;
} PCI_HDA_REGION;


/* Offsets of the registers defined at HDA PCie config space.
 * Details can be found at the Intel I/O Controller Hub 7 (ICH7)/
 * Intel High Definition Audio / AC 97 Programmer's Reference Manual
 * April 2005
*/
#define HDA_OFFSET_PCIE_VID	0x0
#define HDA_OFFSET_PCIE_DID	0x2
#define HDA_OFFSET_PCIE_PCICMD	0x4
#define HDA_OFFSET_PCIE_PCISTS	0x6
#define HDA_OFFSET_PCIE_RID	0x8
#define HDA_OFFSET_PCIE_PI	0x9
#define HDA_OFFSET_PCIE_SCC	0xA
#define HDA_OFFSET_PCIE_BCC	0xB
#define HDA_OFFSET_PCIE_CLS	0xC
#define HDA_OFFSET_PCIE_LT	0xD
#define HDA_OFFSET_PCIE_HEADTYP	0xE
#define HDA_OFFSET_PCIE_HDBARL	0x10
#define HDA_OFFSET_PCIE_HDBARU	0x14
#define HDA_OFFSET_PCIE_SVID	0x2C
#define HDA_OFFSET_PCIE_SID	0x2E
#define HDA_OFFSET_PCIE_CAPPTR	0x34
#define HDA_OFFSET_PCIE_INTLN	0x3C
#define HDA_OFFSET_PCIE_INTPN	0x3D
#define HDA_OFFSET_PCIE_HDCTL	0x40
#define HDA_OFFSET_PCIE_TCSEL	0x44
#define HDA_OFFSET_PCIE_DCKSTS	0x4D
#define HDA_OFFSET_PCIE_PID	0x50
#define HDA_OFFSET_PCIE_PC	0x52
#define HDA_OFFSET_PCIE_PCS	0x54
#define HDA_OFFSET_PCIE_MID	0x60
#define HDA_OFFSET_PCIE_MMC	0x62
#define HDA_OFFSET_PCIE_MMLA	0x64
#define HDA_OFFSET_PCIE_MMUA	0x68
#define HDA_OFFSET_PCIE_MMD	0x6C
#define HDA_OFFSET_PCIE_PXID	0x70
#define HDA_OFFSET_PCIE_PXC	0x72
#define HDA_OFFSET_PCIE_DEVCAP	0x74
#define HDA_OFFSET_PCIE_DEVCTL	0x78
#define HDA_OFFSET_PCIE_DEVS	0x7A
#define HDA_OFFSET_PCIE_VCCAP	0x100
#define HDA_OFFSET_PCIE_PVCCAP1	0x104
#define HDA_OFFSET_PCIE_PVCCAP2	0x108
#define HDA_OFFSET_PCIE_PVCCTL	0x10C
#define HDA_OFFSET_PCIE_PVCSTS	0x10E
#define HDA_OFFSET_PCIE_VC0CAP	0x110
#define HDA_OFFSET_PCIE_VC0CTL	0x114
#define HDA_OFFSET_PCIE_VC0STS	0x11A
#define HDA_OFFSET_PCIE_VciCAP	0x11C
#define HDA_OFFSET_PCIE_VciCTL	0x120
#define HDA_OFFSET_PCIE_VciSTS	0x126
#define HDA_OFFSET_PCIE_RCCAP	0x130
#define HDA_OFFSET_PCIE_ESD	0x134
#define HDA_OFFSET_PCIE_L1DESC	0x140
#define HDA_OFFSET_PCIE_L1ADDL	0x148
#define HDA_OFFSET_PCIE_L1ADDU	0x14C



/* Offsets of the registers defined at HDA
 * compatible controllers.
 * Details can be found at the page 27 of the
 * HDA Specification Rev 1.0a
*/
#define HDA_OFFSET_GCAP	0x0
#define HDA_OFFSET_VMIN	0x2
#define HDA_OFFSET_VMAJ	0x3
#define HDA_OFFSET_OUTPAY 0x4
#define HDA_OFFSET_INPAY 0x6
#define HDA_OFFSET_GCTL	0x8
#define HDA_OFFSET_WAKEEN 0x0C
#define HDA_OFFSET_WAKESTS 0x0E
#define HDA_OFFSET_GSTS	0x10
#define HDA_OFFSET_Rsvd	0x12
#define HDA_OFFSET_OUTSTRMPAY 0x18
#define HDA_OFFSET_INSTRMPAY 0x1A
#define HDA_OFFSET_INTCTL 0x20
#define HDA_OFFSET_INTSTS 0x24
#define HDA_OFFSET_WALCLK 0x30
#define HDA_OFFSET_SSYNC 0x38
#define HDA_OFFSET_CORBLBASE 0x40
#define HDA_OFFSET_CORBUBASE 0x44
#define HDA_OFFSET_CORBWP 0x48
#define HDA_OFFSET_CORBRP 0x4A
#define HDA_OFFSET_CORBCTL 0x4C
#define HDA_OFFSET_CORBSTS 0x4D
#define HDA_OFFSET_CORBSIZE 0x4E
#define HDA_OFFSET_RIRBLBASE 0x50
#define HDA_OFFSET_RIRBUBASE 0x54
#define HDA_OFFSET_RIRBWP 0x58
#define HDA_OFFSET_RINTCNT 0x5A
#define HDA_OFFSET_RIRBCTL 0x5C
#define HDA_OFFSET_RIRBSTS 0x5D
#define HDA_OFFSET_RIRBSIZE 0x5E
#define HDA_OFFSET_ICOI	0x60
#define HDA_OFFSET_ICII	0x64
#define HDA_OFFSET_ICIS	0x68
#define HDA_OFFSET_DPIBLBASE 0x70
#define HDA_OFFSET_DPIBUBASE 0x74

//This is the last offset before the descriptors.
//DO NOT change or remove this definition
#define HDA_OFFSET_SD0CTL 0x80
#define HDA_OFFSET_SD0STS 0x83
#define HDA_OFFSET_SD0LPIB 0x84
#define HDA_OFFSET_SD0CBL 0x88
#define HDA_OFFSET_SD0LVI 0x8C
#define HDA_OFFSET_SD0FIFOS 0x90
#define HDA_OFFSET_SD0FMT 0x92
#define HDA_OFFSET_SD0BDPL 0x98
#define HDA_OFFSET_SD0BDPU 0x9C

/*
 * These definitions are relative to each
 * descriptor.
 * Details can be found at the page 27 of the
 * HDA Specification Rev 1.0a
*/
#define HDA_RELATIVE_OFFSET_SDXCTL 0
#define HDA_RELATIVE_OFFSET_SDXSTS 0x3
#define HDA_RELATIVE_OFFSET_SDXLPIB 0x4
#define HDA_RELATIVE_OFFSET_SDXCBL 0x8
#define HDA_RELATIVE_OFFSET_SDXLVI 0xC
#define HDA_RELATIVE_OFFSET_SDXFIFOS 0x10
#define HDA_RELATIVE_OFFSET_SDXFMT 0x12
#define HDA_RELATIVE_OFFSET_SDXBDPL 0x18
#define HDA_RELATIVE_OFFSET_SDXBDPU 0x1C


//Extract the OSS count from the controller's Global
//capabilities register
#define HDA_OSS_COUNT(GCAP) \
	((GCAP >> 12) & 0xF)

//Extract the ISS count from the controller's Global
//capabilities register
#define HDA_ISS_COUNT(GCAP) \
	((GCAP >> 8) & 0xF)

//Extract the BSS count from the controller's Global
//capabilities register
#define HDA_BSS_COUNT(GCAP) \
	((GCAP >> 3) & 0x1F)


//These macros calculate the offset of the
//stream descriptors as defined at page 27 of the HDA Spec
#define CALCULATE_ISSN_OFFSET(StreamIndex) \
	(HDA_OFFSET_SD0CTL + (StreamIndex * 0x20))

#define CALCULATE_OSSN_OFFSET(StreamIndex,GCAP) (HDA_OFFSET_SD0CTL + \
  (HDA_ISS_COUNT(GCAP) * 0x20 ) + \
  (StreamIndex * 0x20))

#define CALCULATE_BSSN_OFFSET(StreamIndex,GCAP) (HDA_OFFSET_SD0CTL + \
  (HDA_ISS_COUNT(GCAP) * 0x20) + \
  (HDA_OSS_COUNT(GCAP) * 0x20) + \
  (StreamIndex * 0x20))


typedef struct {
	UINT8   StreamReset: 1;
	UINT8   StreamRun: 1;
	UINT8   InterruptOnCompletionEnable: 1;
	UINT8   FIFOErrorInterruptEnable: 1;
	UINT8   DescriptorErrorInterruptEnable: 1;
	UINT8   Reserved1: 3;
	UINT8   Reserved2;
	UINT8   StrippeControl: 2;
	UINT8   TrafficPriority: 1;
	UINT8   BidirectionalDirectionControl: 1;
	UINT8   StreamNumber: 4;

} HDA_CONTROLLER_STREAM_DESCRIPTOR_CONTROL;


typedef struct {
	UINT8   SD0CTL[3]; //details of this property can be found at the HDA_CONTROLLER_STREAM_DESCRIPTOR_CONTROL struct
	UINT8   SD0STS;
	UINT32  SD0LPIB;
	UINT32  SD0CBL;
	UINT16  SD0LVI;
	UINT16  Rsvd8;
	UINT16  SD0FIFOS;
	UINT16  SD0FMT;
	UINT32  Rsvd9;
	UINT32  SD0BDPL;
	UINT32  SD0BDPU;
} HDA_CONTROLLER_STREAM_DESCRIPTOR;



/* Structure representing the HDA controller
 * register set. Details can be found at
 * the page 27 of the HDA Specification Rev 1.0a
*/
typedef struct {
	UINT16  GCAP;
	UINT8   VMIN;
	UINT8   VMAJ;
	UINT16  OUTPAY;
	UINT16  INPAY;
	UINT32  GCTL;
	UINT16  WAKEEN;
	UINT16  STATESTS;
	UINT16  GSTS;
	UINT8   Rsvd0[6];
	UINT16  OUTSTRMPAY;
	UINT16  INSTRMPAY;
	UINT32  Rsvd;
	UINT32  INTCTL;
	UINT32  INTSTS;
	UINT8   Rsvd1[8];
	UINT32  WALCLK;
	UINT32  Rsvd2;
	UINT32  SSYNC;
	UINT32  Rsvd3;
	UINT32  CORBLBASE;
	UINT32  CORBUBASE;
	UINT16  CORBWP;
	UINT16  CORBRP;
	UINT8   CORBCTL;
	UINT8   CORBSTS;
	UINT8   CORBSIZE;
	UINT8   Rsvd4;
	UINT32  RIRBLBASE;
	UINT32  RIRBUBASE;
	UINT16  RIRBWP;
	UINT16  RINTCNT;
	UINT8   RIRBCTL;
	UINT8   RIRBSTS;
	UINT8   RIRBSIZE;
	UINT8   Rsvd5;
	UINT32  ICOI;
	UINT32  ICII;
	UINT16  ICIS;
	UINT8   Rsvd6[6];
	UINT32  DPIBLBASE;
	UINT32  DPIBUBASE;
	UINT8   Rsvd7[8];

	HDA_CONTROLLER_STREAM_DESCRIPTOR* ISS;
	HDA_CONTROLLER_STREAM_DESCRIPTOR* OSS;
	HDA_CONTROLLER_STREAM_DESCRIPTOR* BSS;

} HDA_CONTROLLER_REGISTER_SET;

/*
 * This enumeration contains all verbs
 * that can be executed on widgets present on
 * a HDA compatible codec.
 * Details can be found at the page 218 of the
 * HDA Specification Rev 1.0a
 */
typedef enum {
	HDA_VRB_GET_PARAMETER = 0xF00,
	HDA_VRB_GET_CONNECTION_SELECT = 0xF01,
	HDA_VRB_GET_GET_CONNECTION_LIST_ENTRY = 0xF02,
	HDA_VRB_GET_PROCESSING_STATE = 0xF03,

	/*
	 * These verbs are widget dependent.
	 * We need to find a way to set the correct
	 * verb id at runtime
	 */
	HDA_VRB_GET_COEFFICIENT_INDEX = 0xD,
	HDA_VRB_GET_PROCESSING_COEFFICIENT = 0xC,
	HDA_VRB_GET_AMPLIFIER_GAIN_MUTE = 0xBA0,
	HDA_VRB_GET_STREAM_FORMAT = 0xA,


	HDA_VRB_GET_DIGITAL_CONVERTER_1 = 0xF0D,
	//HDA_VRB_GET_DIGITAL_CONVERTER_2 = 0xF0D,
	//HDA_VRB_GET_DIGITAL_CONVERTER_3 = 0xF0D,
	//HDA_VRB_GET_DIGITAL_CONVERTER_4 = 0xF0D,


	HDA_VRB_GET_POWER_STATE = 0xF05,
	HDA_VRB_GET_CHANNEL_STREAM_ID = 0xF06,
	HDA_VRB_GET_SDI_SELECT = 0xF04,
	HDA_VRB_GET_PIN_WIDGET_CONTROL = 0xF07,
	HDA_VRB_GET_UNSOLICITED_ENABLE = 0xF08,
	HDA_VRB_GET_PIN_SENSE = 0xF09,
	HDA_VRB_GET_EAPD_BTL_ENABLE = 0xF0C,
	HDA_VRB_GET_ALL_GPI_CONTROLS_F10 = 0xF10,
	HDA_VRB_GET_ALL_GPI_CONTROLS_F11 = 0xF11,
	HDA_VRB_GET_ALL_GPI_CONTROLS_F12 = 0xF12,
	HDA_VRB_GET_ALL_GPI_CONTROLS_F13 = 0xF13,
	HDA_VRB_GET_ALL_GPI_CONTROLS_F14 = 0xF14,
	HDA_VRB_GET_ALL_GPI_CONTROLS_F15 = 0xF15,
	HDA_VRB_GET_ALL_GPI_CONTROLS_F16 = 0xF16,
	HDA_VRB_GET_ALL_GPI_CONTROLS_F17 = 0xF17,
	HDA_VRB_GET_ALL_GPI_CONTROLS_F18 = 0xF18,
	HDA_VRB_GET_ALL_GPI_CONTROLS_F19 = 0xF19,
	HDA_VRB_GET_ALL_GPI_CONTROLS_F1A = 0xF1A,
	HDA_VRB_GET_BEEP_GENERATION_CONTROL = 0xF0A,
	HDA_VRB_GET_VOLUME_KNOB_CONTROL = 0xF0F,
	HDA_VRB_GET_IMPLEMENTATION_ID_BYTE_0 = 0xF20,
	//HDA_VRB_GET_IMPLEMENTATION_ID_BYTE_1 = 0xF20,
	//HDA_VRB_GET_IMPLEMENTATION_ID_BYTE_2 = 0xF20,
	//HDA_VRB_GET_IMPLEMENTATION_ID_BYTE_3 = 0xF20,
	HDA_VRB_GET_CONFIG_DEFAULT_BYTE_0 = 0xF1C,
	//HDA_VRB_GET_CONFIG_DEFAULT_BYTE_1 = 0xF1C,
	//HDA_VRB_GET_CONFIG_DEFAULT_BYTE_2 = 0xF1C,
	//HDA_VRB_GET_CONFIG_DEFAULT_BYTE_3 = 0xF1C,
	HDA_VRB_GET_STRIPE_CONTROL = 0xF24,
	HDA_VRB_GET_CONVERTER_CHANNEL_COUNT = 0xF2D,
	HDA_VRB_GET_DIP_SIZE = 0xF2E,
	HDA_VRB_GET_ELD_DATA = 0xF2F,
	HDA_VRB_GET_DIP_INDEX = 0xF30,
	HDA_VRB_GET_DIP_DATA = 0xF31,
	HDA_VRB_GET_DIP_XMITCTRL = 0xF32,
	HDA_VRB_GET_CONTENT_PROTECTION_CONTROL = 0xF33,
	HDA_VRB_GET_ASP_CHANNEL_MAPPING = 0xF34,


	HDA_VRB_SET_CONNECTION_SELECT = 0x701,

	/*
	 * These verbs are widget dependent.
	 * We need to find a way to set the correct
	 * verb id at runtime
	 */
	HDA_VRB_SET_COEFFICIENT_INDEX = 0x5,
	HDA_VRB_SET_PROCESSING_COEFFICIENT = 0x4,
	HDA_VRB_SET_AMPLIFIER_GAIN_MUTE = 0x3B0,
	HDA_VRB_SET_STREAM_FORMAT = 0x2,

	HDA_VRB_SET_DIGITAL_CONVERTER_1 = 0x70D,
	HDA_VRB_SET_DIGITAL_CONVERTER_2 = 0x70E,
	HDA_VRB_SET_DIGITAL_CONVERTER_3 = 0x73E,
	HDA_VRB_SET_DIGITAL_CONVERTER_4 = 0x73F,
	HDA_VRB_SET_POWER_STATE = 0x705,
	HDA_VRB_SET_CHANNEL_STREAM_ID = 0x706,
	HDA_VRB_SET_SDI_SELECT = 0x704,
	HDA_VRB_SET_PIN_WIDGET_CONTROL = 0x707,
	HDA_VRB_SET_UNSOLICITED_ENABLE = 0x708,
	HDA_VRB_SET_PIN_SENSE = 0x709,
	HDA_VRB_SET_EAPD_BTL_ENABLE = 0x70C,
	HDA_VRB_SET_ALL_GPI_CONTROLS_710 = 0x710,
	HDA_VRB_SET_ALL_GPI_CONTROLS_711 = 0x711,
	HDA_VRB_SET_ALL_GPI_CONTROLS_712 = 0x712,
	HDA_VRB_SET_ALL_GPI_CONTROLS_713 = 0x713,
	HDA_VRB_SET_ALL_GPI_CONTROLS_714 = 0x714,
	HDA_VRB_SET_ALL_GPI_CONTROLS_715 = 0x715,
	HDA_VRB_SET_ALL_GPI_CONTROLS_716 = 0x716,
	HDA_VRB_SET_ALL_GPI_CONTROLS_717 = 0x717,
	HDA_VRB_SET_ALL_GPI_CONTROLS_718 = 0x718,
	HDA_VRB_SET_ALL_GPI_CONTROLS_719 = 0x719,
	HDA_VRB_SET_ALL_GPI_CONTROLS_71A = 0x71A,
	HDA_VRB_SET_BEEP_GENERATION_CONTROL = 0x70A,
	HDA_VRB_SET_VOLUME_KNOB_CONTROL = 0x70F,
	HDA_VRB_SET_IMPLEMENTATION_ID_BYTE_0 = 0x720,
	HDA_VRB_SET_IMPLEMENTATION_ID_BYTE_1 = 0x721,
	HDA_VRB_SET_IMPLEMENTATION_ID_BYTE_2 = 0x722,
	HDA_VRB_SET_IMPLEMENTATION_ID_BYTE_3 = 0x723,
	HDA_VRB_SET_CONFIG_DEFAULT_BYTE_0 = 0x71C,
	HDA_VRB_SET_CONFIG_DEFAULT_BYTE_1 = 0x71D,
	HDA_VRB_SET_CONFIG_DEFAULT_BYTE_2 = 0x71E,
	HDA_VRB_SET_CONFIG_DEFAULT_BYTE_3 = 0x723,
	HDA_VRB_SET_STRIPE_CONTROL = 0x724,
	HDA_VRB_SET_CONVERTER_CHANNEL_COUNT = 0x72D,
	HDA_VRB_SET_DIP_INDEX = 0x730,
	HDA_VRB_SET_DIP_DATA = 0x731,
	HDA_VRB_SET_DIP_XMITCTRL = 0x732,
	HDA_VRB_SET_CONTENT_PROTECTION_CONTROL = 0x733,
	HDA_VRB_SET_ASP_CHANNEL_MAPPING = 0x734,


	HDA_VRB_SET_RESET = 0x7FF
} HDA_VERB;

/*
 * This macro is used when a Immediate command needs to
 * be started
 */
#define HDA_START_PROCESSING_IMMEDIATE_COMMAND 0x01

/*
 * This macro extracts the total node count from a
 * SubordinateNodeCount property
 */
#define HDA_SUB_NODE_COUNT_TOTAL_NODE(Subordinate) \
	(Subordinate & 0xFF)

/*
 * This macro extracts the total node count from a
 * SubordinateNodeCount property
 */
#define HDA_SUB_NODE_COUNT_START_NODE(Subordinate) \
	((Subordinate >> 16) & 0xFF)

/*
 * This macro extracts the node type from a
 * FuncttionGroup property
 */
#define HDA_NODE_TYPE(FunctionGroupType) \
	(FunctionGroupType & 0xFF)

/*
 * This macro is used to check if a given function group
 * can generate unsolicited responses
 */
#define HDA_UNSOLICITED_RESPONSE_CAPABLE(FunctionGroupType) \
	((FunctionGroupType >> 8) & 0x1)

/*
 * This macro extracts the widget type from the
 * widget capabilities information of a widget
 */
#define HDA_WIDGET_TYPE(WidgetCap) \
	((WidgetCap >> 20) & 0xF)

/*
 * This structure represents an HDA command
 * with 8 bits payload.
 * Details can be found at the page 141 of the
 * HDA Specification Rev 1.0a
 */
typedef struct {
	UINT32 VerbPayload: 8;
	UINT32 VerbIdent: 12; //the verb
	UINT32 NID: 8; //node id
	UINT32 CAd: 4; //Codec address
	UINT8 Reserved; //Reserved

} HDA_COMMAND_FIELD_8BIT_PAYLOAD;

/*
 * This structure represents an HDA command
 * with 16 bits payload.
 * Details can be found at the page 141 of the
 * HDA Specification Rev 1.0a
 */
typedef struct {
	UINT32 VerbPayload: 16;
	UINT32 VerbIdent: 4; //the verb
	UINT32 NID: 8; //node id
	UINT32 CAd: 4; //Codec address
	UINT8 Reserved; //Reserved

} HDA_COMMAND_FIELD_16BIT_PAYLOAD;


/***********************************************
 * This structure represents the stream format,
 * as defined at page 58 of the HDA Specification
 * Rev 1.0a
 * This struct is used to configure the widgets
 * Do not confuse with the struct
 * HDA_STREAM_DESCRIPTOR_FORMAT
 **********************************************/
typedef struct {
	UINT16 NumberOfChannels: 4;
	UINT16 BitsPerSample: 3;
	UINT16 Reserved: 1;
	UINT16 SampleBaseRateBaseDivisor: 3;
	UINT16 SampleBaseRateMultiple: 3;
	UINT16 SampleBaseRate: 1;
	UINT16 StreamType: 1;
} HDA_STREAM_FORMAT;

/***********************************************
 * This structure represents the stream format,
 * as defined at page 48 of the HDA Specification
 * Rev 1.0a
 * This struct is used to configure the controller
 * Do not confuse with the struct HDA_STREAM_FORMAT
 **********************************************/
typedef struct {
	UINT16 NumberOfChannels: 4;
	UINT16 BitsPerSample: 3;
	UINT16 Reserved: 1;
	UINT16 SampleBaseRateBaseDivisor: 3;
	UINT16 SampleBaseRateMultiple: 3;
	UINT16 SampleBaseRate: 1;
	UINT16 Reserved2: 1;
} HDA_STREAM_DESCRIPTOR_FORMAT;

#define PCM_STRUCT_BITS_PER_SAMPLE_8 0
#define PCM_STRUCT_BITS_PER_SAMPLE_16 1
#define PCM_STRUCT_BITS_PER_SAMPLE_20 2
#define PCM_STRUCT_BITS_PER_SAMPLE_24 3
#define PCM_STRUCT_BITS_PER_SAMPLE_32 4

#define PCM_STRUCT_SAMPLE_BASE_DIV_BY_1 0
#define PCM_STRUCT_SAMPLE_BASE_DIV_BY_2 1
#define PCM_STRUCT_SAMPLE_BASE_DIV_BY_3 2
#define PCM_STRUCT_SAMPLE_BASE_DIV_BY_4 3
#define PCM_STRUCT_SAMPLE_BASE_DIV_BY_5 4
#define PCM_STRUCT_SAMPLE_BASE_DIV_BY_6 5
#define PCM_STRUCT_SAMPLE_BASE_DIV_BY_7 6

//192 kHz, 176.4 kHz
#define PCM_STRUCT_SAMPLE_BASE_MULTIPLE_X4 3
//144 kHz
#define PCM_STRUCT_SAMPLE_BASE_MULTIPLE_X3 2
//96 kHz, 88.2 kHz, 32 kHz
#define PCM_STRUCT_SAMPLE_BASE_MULTIPLE_X2 1
//48KHz/44.1kHz or less
#define PCM_STRUCT_SAMPLE_BASE_MULTIPLE_48_OR_LESS 0

#define PCM_STRUCT_SAMPLE_BASE_44_1KHZ 1
#define PCM_STRUCT_SAMPLE_BASE_48KHZ 0

#define PCM_STRUCT_TYPE_PCM 0
#define PCM_STRUCT_TYPE_NON_PCM 1

/***********************************************/


/*
 * This enumeration contains all possible parameters
 * to be used when executing a HDA_VRB_GET_PARAMETER (0xF00)
 * call.
 * Details can be found at the page 217 of the
 * HDA Specification Rev 1.0a
 */
typedef enum  {
	HDA_PARAM_VENDOR_ID = 0x00,
	HDA_PARAM_REVISION_ID = 0x02,
	HDA_PARAM_SUBORDINATE_NODE_COUNT = 0x04,
	HDA_PARAM_FUNCTION_GROUP_TYPE = 0x05,
	HDA_PARAM_AUDIO_FUNC_CAP = 0x08,
	HDA_PARAM_AUDIO_WIDGET_CAP = 0x09,
	HDA_PARAM_SAMPLE_SIZE_RATE_CAP = 0x0A,
	HDA_PARAM_STREAM_FORMATS = 0x0B,
	HDA_PARAM_PIN_CAP = 0x0C,
	HDA_PARAM_INPUT_AMP_CAP = 0x0D,
	HDA_PARAM_OUTPUT_AMP_CAP = 0x12,
	HDA_PARAM_CONNECTION_LIST_LENGTH = 0x0E,
	HDA_PARAM_SUPPORTED_POWER_STATES = 0x0F,
	HDA_PARAM_PROCESSING_CAP = 0x10,
	HDA_PARAM_GPIO_COUNT = 0x11,
	HDA_PARAM_VOLUME_KNOB_CAP = 0x13
} HDA_PARAMETER;

/*
 * This enumeration contains all possible parameters
 * widget types present at  the HDA Specification
 * Rev 1.0a
 */
typedef enum  {
	HDA_WIDGET_TYPE_AUDIO_OUTPUT = 0x0,
	HDA_WIDGET_TYPE_AUDIO_INPUT = 0x1,
	HDA_WIDGET_TYPE_AUDIO_MIXER = 0x2,
	HDA_WIDGET_TYPE_AUDIO_SELECTOR = 0x3,
	HDA_WIDGET_TYPE_AUDIO_PIN_CONPLEX = 0x4,
	HDA_WIDGET_TYPE_AUDIO_POWER = 0x5,
	HDA_WIDGET_TYPE_AUDIO_VOLUME_KNOB = 0x6,
	HDA_WIDGET_TYPE_AUDIO_BEEP_GENERATOR = 0x7,
	HDA_WIDGET_TYPE_AUDIO_VENDOR_DEFINED = 0xF
} HDA_WIDGET_TYPE;

/*
 * This enumeration contains the function group
 * types defined by the HDA Specification
 */
typedef enum  {
	HDA_FUNCTION_GROUP_TYPE_AUDIO = 0x1,
	HDA_FUNCTION_GROUP_TYPE_MODEM = 0x2
} HDA_FUNCTION_GROUP_TYPE;

/*
 * This enumeration contains the possible node types
 * available at a codec.
 */
typedef enum {
	HDA_NODE_ROOT = 0x0,
	HDA_NODE_FUNCTION_GROUP = 0x1,
	HDA_NODE_WIDGET = 0x2,
	HDA_UNKNOWN = 0xFF
} HDA_NODE_TYPE;


/*
 * This can be used when a verb does not need
 * any kind of payload
 */
#define HDA_VRB_EMPTY_PAYLOAD 0

/*
 * This enumeration contains the possible power states a
 * node can present based on the HDA specification.
 *
 * Details can be found at the page 151 of the
 * HDA Specification Rev 1.0a
 *
 */
typedef enum {
	HDA_POWER_D0 = 0x0,
	HDA_POWER_D1 = 0x1,
	HDA_POWER_D2 = 0x2,
	HDA_POWER_D3 = 0x3,
	HDA_POWER_D3_COLD = 0x4,
} HDA_POWER_STATE;




/*
 * This structure represents a node that can be
 * available at a HDA codec. A node can be a
 * widget or a function group. Depending on the
 * node type some information can be disregarded
 */
struct  Node{
  UINT32 NodeId;
  HDA_NODE_TYPE NodeType;
  HDA_WIDGET_TYPE WidgetType;
  UINT32 VendorId;
  UINT32 RevisionId;
  UINT32 StartingChildNodeAddess;
  UINT32 SubordinateNodeCount;
  UINT32 FunctionGroupType;
  UINT32 FuncCap;
  UINT32 WidgetCap;
  UINT32 SampleSizeRateCap;
  UINT32 StreamFormat;
  UINT32 PinCap;
  UINT32 InputAmpCap;
  UINT32 OutputAmpCap;
  UINT32 ConnectionListLength;
  UINT32 SupportedPowerStates;
  UINT32 ProcessingCap;
  UINT32 GPIOCount;
  UINT32 VolKnobCap;

  //This register information is filed
  //using the F05 verb (GetPowerState)
  UINT32 PowerState;

  //This register information is filed
  //using the 0xB verb (Amplifier Gain/Mute)
  UINT32 RightGain;
  UINT32 LeftGain;


  ////This register information is filed
  //using the F05 verb (GetStreamId)
  UINT32 ChannelStreamId;



  struct Node *ChildNodes;
};


//Page 142 of the HDA Specification
typedef struct {
	UINT32 Response;
	UINT8 Reserved: 2;
	UINT8 UnSol: 1;
	UINT8 Valid: 1;
	UINT8 UnUsed: 4;

} HDA_RESPONSE_FIELD;


typedef struct {

		UINT64 Address;
		UINT32 Length;
		UINT32 IntrptOnComp: 1;
		UINT32 Resv: 31;

} HDA_BUFFER_DESCRIPTOR_LIST_ENTRY;


typedef struct {
		HDA_BUFFER_DESCRIPTOR_LIST_ENTRY
			BDLEntry[HDA_BUFFER_DESC_LIST_MAX_ENTRIES];
} HDA_BUFFER_DESCRIPTOR_LIST;

EFI_STATUS InitHdaLib ();


EFI_STATUS AllocateCORBBuffer(PCI_HDA_REGION* PcieDeviceConfigSpace);

EFI_STATUS AllocateRIRBBuffer(PCI_HDA_REGION* PcieDeviceConfigSpace);

EFI_STATUS FillCodecNode(PCI_HDA_REGION* PcieDeviceConfigSpace,
		                  UINT32 CurrentNodeId,
		                  HDA_NODE_TYPE NodeType,
		                  struct Node *CurrentNode);

EFI_STATUS GetNodeById(struct Node *RootNode,
		               UINT32 NodeIdToSearch,
		               struct Node* NodeDetected);

EFI_STATUS GetCodecTree (PCI_HDA_REGION* PcieDeviceConfigSpace,
		struct Node *RootNode);

EFI_STATUS ReleaseCodecTree (PCI_HDA_REGION* PcieDeviceConfigSpace,
		struct Node *RootNode);

EFI_STATUS GetCodecData8BitPayload (PCI_HDA_REGION* PcieDeviceConfigSpace,
		      UINT8 CodecAddress, UINT8 NodeId,
		      HDA_VERB Verb, UINT8 VerbPayload,
		      UINT32 *Response);

EFI_STATUS GetCodecData8BitPayloadCorbRirb (
  PCI_HDA_REGION* PcieDeviceConfigSpace,
  UINT8 CodecAddress, UINT8 NodeId,
  HDA_VERB Verb, UINT8 VerbPayload,
  UINT32 *Response);

EFI_STATUS GetCodecData16BitPayloadCorbRirb (
  PCI_HDA_REGION* PcieDeviceConfigSpace,
  UINT8 CodecAddress, UINT8 NodeId,
  HDA_VERB Verb, UINT16 VerbPayload,
  UINT32 *Response);


EFI_STATUS SendCommandToAllWidgets8BitPayload (
		PCI_HDA_REGION* PcieDeviceConfigSpace,
		HDA_VERB Verb, UINT8 VerbPayload);

EFI_STATUS SendCommandToAllWidgets16BitPayload (
		PCI_HDA_REGION* PcieDeviceConfigSpace,
		HDA_VERB Verb, UINT16 VerbPayload);

EFI_STATUS GetCodecData16BitPayload (PCI_HDA_REGION* PcieDeviceConfigSpace,
		      UINT8 CodecAddress, UINT8 NodeId,
		      HDA_VERB Verb, UINT16 VerbPayload,
		      UINT32 *Response);

UINT32 GetAmplifierGain(PCI_HDA_REGION *PcieDeviceConfigSpace,
		                 UINT8 NodeId, BOOLEAN InputOutput,
		                 BOOLEAN LeftRight);

EFI_STATUS DisablePcieInterrupts (PCI_HDA_REGION* PcieDeviceConfigSpace);

EFI_STATUS EnablePcieNoSnoop (PCI_HDA_REGION* PcieDeviceConfigSpace);

EFI_STATUS AddDestriptorListEntryOss0(
		PCI_HDA_REGION* PcieDeviceConfigSpace,
		HDA_CONTROLLER_REGISTER_SET* ControllerRegisterSet,
		UINT64 DataAddress,
		UINT32 DataLength,
		UINT8 BdlEntryIndex,
		UINT32 SdxLastValidIndex);

EFI_STATUS AllocateStreamsPages(
		PCI_HDA_REGION* PcieDeviceConfigSpace,
		HDA_CONTROLLER_REGISTER_SET* ControllerRegisterSet);


//EFI_STATUS TurnOn (PCI_HDA_REGION* PcieDevConfSpace);

//EFI_STATUS TurnOff (PCI_HDA_REGION* PcieDevConfSpace);

#endif
