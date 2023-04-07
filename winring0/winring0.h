//-----------------------------------------------------------------------------
//     Author : hiyohiyo
//       Mail : hiyohiyo@crystalmark.info
//        Web : http://openlibsys.org/
//    License : The modified BSD license
//
//                     Copyright 2007-2008 OpenLibSys.org. All rights reserved.
//-----------------------------------------------------------------------------

#pragma once

#include <stdint.h>
#include <windows.h>

//-----------------------------------------------------------------------------
//
// The Device type codes form 32768 to 65535 are for customer use.
//
//-----------------------------------------------------------------------------

#define OLS_TYPE 40000

// HwRwDrv.sys
#define OLS_DRIVER_ID							"HwRwDrv"
#define OLS_DRIVER_NAME							"HwRwDrv"
#define OLS_DRIVER_OBJ							"\\\\.\\HwRwDrv"
// WinRing0.sys
#define OLS_ALT_DRIVER_ID						"WinRing0_1_2_0"
#define OLS_ALT_DRIVER_NAME						"WinRing0"
#define OLS_ALT_DRIVER_OBJ						"\\\\.\\WinRing0_1_2_0"

// The IOCTL function codes from 0x800 to 0xFFF are for customer use.
#define IOCTL_OLS_GET_DRIVER_VERSION \
	CTL_CODE(OLS_TYPE, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_OLS_GET_REFCOUNT \
	CTL_CODE(OLS_TYPE, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_OLS_READ_MSR \
	CTL_CODE(OLS_TYPE, 0x821, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_OLS_WRITE_MSR \
	CTL_CODE(OLS_TYPE, 0x822, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_OLS_READ_PMC \
	CTL_CODE(OLS_TYPE, 0x823, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_OLS_HALT \
	CTL_CODE(OLS_TYPE, 0x824, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_OLS_READ_IO_PORT \
	CTL_CODE(OLS_TYPE, 0x831, METHOD_BUFFERED, FILE_READ_ACCESS)

#define IOCTL_OLS_WRITE_IO_PORT \
	CTL_CODE(OLS_TYPE, 0x832, METHOD_BUFFERED, FILE_WRITE_ACCESS)

#define IOCTL_OLS_READ_IO_PORT_BYTE \
	CTL_CODE(OLS_TYPE, 0x833, METHOD_BUFFERED, FILE_READ_ACCESS)

#define IOCTL_OLS_READ_IO_PORT_WORD \
	CTL_CODE(OLS_TYPE, 0x834, METHOD_BUFFERED, FILE_READ_ACCESS)

#define IOCTL_OLS_READ_IO_PORT_DWORD \
	CTL_CODE(OLS_TYPE, 0x835, METHOD_BUFFERED, FILE_READ_ACCESS)

#define IOCTL_OLS_WRITE_IO_PORT_BYTE \
	CTL_CODE(OLS_TYPE, 0x836, METHOD_BUFFERED, FILE_WRITE_ACCESS)

#define IOCTL_OLS_WRITE_IO_PORT_WORD \
	CTL_CODE(OLS_TYPE, 0x837, METHOD_BUFFERED, FILE_WRITE_ACCESS)

#define IOCTL_OLS_WRITE_IO_PORT_DWORD \
	CTL_CODE(OLS_TYPE, 0x838, METHOD_BUFFERED, FILE_WRITE_ACCESS)

#define IOCTL_OLS_READ_MEMORY \
	CTL_CODE(OLS_TYPE, 0x841, METHOD_BUFFERED, FILE_READ_ACCESS)

#define IOCTL_OLS_WRITE_MEMORY \
	CTL_CODE(OLS_TYPE, 0x842, METHOD_BUFFERED, FILE_WRITE_ACCESS)

#define IOCTL_OLS_READ_PCI_CONFIG \
	CTL_CODE(OLS_TYPE, 0x851, METHOD_BUFFERED, FILE_READ_ACCESS)

#define IOCTL_OLS_WRITE_PCI_CONFIG \
	CTL_CODE(OLS_TYPE, 0x852, METHOD_BUFFERED, FILE_WRITE_ACCESS)

// PCI Error Code
#define OLS_ERROR_PCI_BUS_NOT_EXIST		(0xE0000001L)
#define OLS_ERROR_PCI_NO_DEVICE			(0xE0000002L)
#define OLS_ERROR_PCI_WRITE_CONFIG		(0xE0000003L)
#define OLS_ERROR_PCI_READ_CONFIG		(0xE0000004L)

// Bus Number, Device Number and Function Number to PCI Device Address
#define PciBusDevFunc(Bus, Dev, Func)	((Bus&0xFF)<<8) | ((Dev&0x1F)<<3) | (Func&7)
// PCI Device Address to Bus Number
#define PciGetBus(address)				((address>>8) & 0xFF)
// PCI Device Address to Device Number
#define PciGetDev(address)				((address>>3) & 0x1F)
// PCI Device Address to Function Number
#define PciGetFunc(address)				(address&7)

#pragma pack(push,4)

typedef struct  _OLS_WRITE_MSR_INPUT {
	ULONG		Register;
	ULARGE_INTEGER	Value;
}   OLS_WRITE_MSR_INPUT;

typedef struct  _OLS_WRITE_IO_PORT_INPUT {
	ULONG	PortNumber;
	union {
		ULONG   LongData;
		USHORT  ShortData;
		UCHAR   CharData;
	};
}   OLS_WRITE_IO_PORT_INPUT;

typedef struct  _OLS_READ_PCI_CONFIG_INPUT {
	ULONG PciAddress;
	ULONG PciOffset;
}   OLS_READ_PCI_CONFIG_INPUT;

typedef struct  _OLS_WRITE_PCI_CONFIG_INPUT {
	ULONG PciAddress;
	ULONG PciOffset;
	UCHAR Data[1];
}   OLS_WRITE_PCI_CONFIG_INPUT;

typedef LARGE_INTEGER PHYSICAL_ADDRESS;

typedef struct  _OLS_READ_MEMORY_INPUT {
	PHYSICAL_ADDRESS Address;
	ULONG UnitSize;
	ULONG Count;
}   OLS_READ_MEMORY_INPUT;

typedef struct  _OLS_WRITE_MEMORY_INPUT {
	PHYSICAL_ADDRESS Address;
	ULONG UnitSize;
	ULONG Count;
	UCHAR Data[1];
}   OLS_WRITE_MEMORY_INPUT;

#pragma pack(pop)

struct wr0_drv_t
{
	LPCSTR driver_name;
	LPCSTR driver_id;
	LPCSTR driver_obj;
	CHAR driver_path[MAX_PATH + 1];
	SC_HANDLE scManager;
	SC_HANDLE scDriver;
	HANDLE hhDriver;
	int errorcode;
};

int cpu_rdmsr(struct wr0_drv_t* driver, uint32_t msr_index, uint64_t* result);
uint8_t io_inb(struct wr0_drv_t* drv, uint16_t port);
uint16_t io_inw(struct wr0_drv_t* drv, uint16_t port);
uint32_t io_inl(struct wr0_drv_t* drv, uint16_t port);
void io_outb(struct wr0_drv_t* drv, uint16_t port, uint8_t value);
void io_outw(struct wr0_drv_t* drv, uint16_t port, uint16_t value);
void io_outl(struct wr0_drv_t* drv, uint16_t port, uint32_t value);
int pci_conf_read(struct wr0_drv_t* drv, uint32_t addr, uint32_t reg, void* value, uint32_t size);
uint8_t pci_conf_read8(struct wr0_drv_t* drv, uint32_t addr, uint32_t reg);
uint16_t pci_conf_read16(struct wr0_drv_t* drv, uint32_t addr, uint32_t reg);
uint32_t pci_conf_read32(struct wr0_drv_t* drv, uint32_t addr, uint32_t reg);
int pci_conf_write(struct wr0_drv_t* drv, uint32_t addr, uint32_t reg, void* value, uint32_t size);
void pci_conf_write8(struct wr0_drv_t* drv, uint32_t addr, uint32_t reg, uint8_t value);
void pci_conf_write16(struct wr0_drv_t* drv, uint32_t addr, uint32_t reg, uint16_t value);
void pci_conf_write32(struct wr0_drv_t* drv, uint32_t addr, uint32_t reg, uint32_t value);
uint32_t pci_find_by_id(struct wr0_drv_t* drv, uint16_t vid, uint16_t did, uint8_t index);
uint32_t pci_find_by_class(struct wr0_drv_t* drv, uint8_t base, uint8_t sub, uint8_t prog, uint8_t index);
DWORD phymem_read(struct wr0_drv_t* drv, DWORD_PTR address, PBYTE buffer, DWORD count, DWORD unitSize);

struct wr0_drv_t* wr0_driver_open(void);
int wr0_driver_close(struct wr0_drv_t* drv);
