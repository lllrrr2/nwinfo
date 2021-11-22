// SPDX-License-Identifier: Unlicense
#pragma once

#include <stdio.h>
#include <stdint.h>
#include <windows.h>

void ObtainPrivileges(LPCTSTR privilege);
const char* GetHumanSize(UINT64 size, const char* human_sizes[6], UINT64 base);
PVOID GetAcpi(DWORD TableId);
UINT8 AcpiChecksum(void* base, UINT size);
void TrimString(CHAR* String);
int GetRegDwordValue(HKEY Key, LPCSTR SubKey, LPCSTR ValueName, DWORD* pValue);
extern CHAR* IDS;
extern DWORD IDS_SIZE;
void FindId(CONST CHAR* v, CONST CHAR* d, CONST CHAR* s, int usb);
const CHAR* GuidToStr(UCHAR Guid[16]);

// NT5 compatible
const char* nt5_inet_ntop4(const unsigned char* src, char* dst, size_t size);
const char* nt5_inet_ntop6(const unsigned char* src, char* dst, size_t size);
void NT5ConvertLengthToIpv4Mask(ULONG MaskLength, ULONG* Mask);
ULONGLONG NT5GetTickCount(void);
UINT NT5EnumSystemFirmwareTables(DWORD FirmwareTableProviderSignature, PVOID pFirmwareTableEnumBuffer, DWORD BufferSize);
UINT NT5GetSystemFirmwareTable(DWORD FirmwareTableProviderSignature, DWORD FirmwareTableID,
	PVOID pFirmwareTableBuffer, DWORD BufferSize);

void nwinfo_sys(void);
void nwinfo_cpuid(int debug_level);
void nwinfo_acpi(DWORD signature);
void nwinfo_network(int active);
void nwinfo_smbios(UINT8 type);
void nwinfo_disk(void);
void nwinfo_display(void);
void nwinfo_pci(const GUID* Guid, const CHAR *PciClass);
void nwinfo_usb(const GUID* Guid);

#pragma pack(1)

struct acpi_table_header
{
	uint8_t signature[4];
	uint32_t length;
	uint8_t revision;
	uint8_t checksum;
	uint8_t oemid[6];
	uint8_t oemtable[8];
	uint32_t oemrev;
	uint8_t creator_id[4];
	uint32_t creator_rev;
};

// Microsoft Data Management table structure
struct acpi_msdm
{
	struct acpi_table_header header;
	uint32_t version;
	uint32_t reserved;
	uint32_t data_type;
	uint32_t data_reserved;
	uint32_t data_length;
	char data[29];
};

struct acpi_madt_entry_header
{
	uint8_t type;
	uint8_t len;
};

struct acpi_madt
{
	struct acpi_table_header header;
	uint32_t lapic_addr;
	uint32_t flags;
	struct acpi_madt_entry_header entries[0];
};

struct acpi_bgrt
{
	struct acpi_table_header header;
	// 2-bytes (16 bit) version ID. This value must be 1.
	uint16_t version;
	// 1-byte status field indicating current status about the table.
	// Bits[7:1] = Reserved (must be zero)
	// Bit [0] = Valid. A one indicates the boot image graphic is valid.
	uint8_t status;
	// 0 = Bitmap
	// 1 - 255  Reserved (for future use)
	uint8_t type;
	// physical address pointing to the firmware's in-memory copy of the image.
	uint64_t addr;
	// (X, Y) display offset of the top left corner of the boot image.
	// The top left corner of the display is at offset (0, 0).
	uint32_t x;
	uint32_t y;
};

struct acpi_wpbt
{
	struct acpi_table_header header;
	/* The size of the handoff memory buffer
	 * containing a platform binary.*/
	uint32_t binary_size;
	/* The 64-bit physical address of a memory
	 * buffer containing a platform binary. */
	uint64_t binary_addr;
	/* Description of the layout of the handoff memory buffer.
	 * Possible values include:
	 * 1 �C Image location points to a single
	 *     Portable Executable (PE) image at
	 *     offset 0 of the specified memory
	 *     location. The image is a flat image
	 *     where sections have not been expanded
	 *     and relocations have not been applied.
	 */
	uint8_t content_layout;
	/* Description of the content of the binary
	 * image and the usage model of the
	 * platform binary. Possible values include:
	 * 1 �C The platform binary is a native usermode application
	 *     that should be executed by the Windows Session
	 *     Manager during operating system initialization.
	 */
	uint8_t content_type;
	uint16_t cmdline_length;
	uint16_t cmdline[0];
};

struct acpi_gas
{
	uint8_t address_space;
	uint8_t bit_width;
	uint8_t bit_offset;
	uint8_t access_size;
	uint64_t address;
};

struct acpi_fadt
{
	struct acpi_table_header header;
	uint32_t facs_addr;
	uint32_t dsdt_addr;
	uint8_t  reserved; //INT_MODEL
	uint8_t  preferred_pm_profile;
	uint16_t sci_int;
	uint32_t smi_cmd;
	uint8_t  acpi_enable;
	uint8_t  acpi_disable;
	uint8_t  s4bios_req;
	uint8_t  pstate_cnt;
	uint32_t pm1a_evt_blk;
	uint32_t pm1b_evt_blk;
	uint32_t pm1a_cnt_blk;
	uint32_t pm1b_cnt_blk;
	uint32_t pm2_cnt_blk;
	uint32_t pm_tmr_blk;
	uint32_t gpe0_blk;
	uint32_t gpe1_blk;
	uint8_t  pm1__evt_len;
	uint8_t  pm1_cnt_len;
	uint8_t  pm2_cnt_len;
	uint8_t  pm_tmr_len;
	uint8_t  gpe0_len;
	uint8_t  gpe1_len;
	uint8_t  gpe1_base;
	uint8_t  cst_cnt;
	uint16_t p_lvl2_lat;
	uint16_t p_lvl3_lat;
	uint16_t flush_size;
	uint16_t flush_stride;
	uint8_t  duty_offset;
	uint8_t  duty_width;
	uint8_t  day_alarm;
	uint8_t  month_alarm;
	uint8_t  century;
	// reserved in ACPI 1.0; used since ACPI 2.0+
	uint16_t iapc_boot_arch;
	uint8_t  reserved2;
	uint32_t flags;
	struct acpi_gas reset_reg;
	uint8_t  reset_value;
	uint16_t arm_boot_arch;
	uint8_t  fadt_minor_ver;
	// 64bit pointers - Available on ACPI 2.0+
	uint64_t facs_xaddr;
	uint64_t dsdt_xaddr;
	struct acpi_gas x_pm1a_evt_blk;
	struct acpi_gas x_pm1b_evt_blk;
	struct acpi_gas x_pm1a_cnt_blk;
	struct acpi_gas x_pm1b_cnt_blk;
	struct acpi_gas x_pm2_cnt_blk;
	struct acpi_gas x_pm_tmr_blk;
	struct acpi_gas x_gpe0_blk;
	struct acpi_gas x_gpe1_blk;
};

struct RawSMBIOSData
{
	BYTE Used20CallingMethod;
	BYTE MajorVersion;
	BYTE MinorVersion;
	BYTE DmiRevision;
	DWORD Length;
	BYTE Data[];
};

typedef struct _SMBIOSHEADER_
{
	BYTE Type;
	BYTE Length;
	WORD Handle;
} SMBIOSHEADER, * PSMBIOSHEADER;

typedef struct _TYPE_0_ {
	SMBIOSHEADER	Header;
	UCHAR	Vendor;
	UCHAR	Version;
	UINT16	StartingAddrSeg;
	UCHAR	ReleaseDate;
	UCHAR	ROMSize;
	ULONG64 Characteristics;
	UCHAR	Extension[2]; // spec. 2.3
	UCHAR	MajorRelease;
	UCHAR	MinorRelease;
	UCHAR	ECFirmwareMajor;
	UCHAR	ECFirmwareMinor;
} BIOSInfo, * PBIOSInfo;

typedef struct _TYPE_1_ {
	SMBIOSHEADER	Header;
	UCHAR	Manufacturer;
	UCHAR	ProductName;
	UCHAR	Version;
	UCHAR	SN;
	UCHAR	UUID[16];
	UCHAR	WakeUpType;
	UCHAR	SKUNumber;
	UCHAR	Family;
} SystemInfo, * PSystemInfo;

typedef struct _TYPE_2_ {
	SMBIOSHEADER	Header;
	UCHAR	Manufacturer;
	UCHAR	Product;
	UCHAR	Version;
	UCHAR	SN;
	UCHAR	AssetTag;
	UCHAR	FeatureFlags;
	UCHAR	LocationInChassis;
	UINT16	ChassisHandle;
	UCHAR	Type;
	UCHAR	NumObjHandle;
	UINT16* pObjHandle;
} BoardInfo, * PBoardInfo;

typedef struct _TYPE_3_ {
	SMBIOSHEADER Header;
	UCHAR	Manufacturer;
	UCHAR	Type;
	UCHAR	Version;
	UCHAR	SN;
	UCHAR	AssetTag;
	UCHAR	BootupState;
	UCHAR	PowerSupplyState;
	UCHAR	ThermalState;
	UCHAR	SecurityStatus;
	ULONG32	OEMDefine;
	UCHAR	Height;
	UCHAR	NumPowerCord;
	UCHAR	ElementCount;
	UCHAR	ElementRecordLength;
	UCHAR	pElements;
} SystemEnclosure, * PSystemEnclosure;

typedef struct _TYPE_4_ {
	SMBIOSHEADER Header;
	UCHAR	SocketDesignation;
	UCHAR	Type;
	UCHAR	Family;
	UCHAR	Manufacturer;
	ULONG64 ID;
	UCHAR	Version;
	UCHAR	Voltage;
	UINT16	ExtClock;
	UINT16	MaxSpeed;
	UINT16	CurrentSpeed;
	UCHAR   Status;
	UCHAR   ProcessorUpgrade;
	UINT16  L1CacheHandle;
	UINT16  L2CacheHandle;
	UINT16  L3CacheHandle;
	UCHAR   Serial;
	UCHAR   AssetTag;
	UCHAR   PartNum;
	UCHAR   CoreCount;
	UCHAR   CoreEnabled;
	UCHAR   ThreadCount;
	UINT16  ProcessorChar;
	UINT16  Family2;
	UINT16  CoreCount2;
	UINT16  CoreEnabled2;
	UINT16  ThreadCount2;
} ProcessorInfo, * PProcessorInfo;

typedef struct _TYPE_5_ {
	SMBIOSHEADER Header;
	UCHAR	ErrDetecting;
	UCHAR	ErrCorrection;
	UCHAR	SupportedInterleave;
	UCHAR	CurrentInterleave;
	UCHAR	MaxMemModuleSize;
	UINT16	SupportedSpeeds;
	UINT16	SupportedMemTypes;
	UCHAR	MemModuleVoltage;
	UCHAR	NumOfSlots;
} MemCtrlInfo, * PMemCtrlInfo;

typedef struct _TYPE_6_ {
	SMBIOSHEADER Header;
	UCHAR	SocketDesignation;
	UCHAR	BankConnections;
	UCHAR	CurrentSpeed;
	UINT16	CurrentMemType;
	UCHAR	InstalledSize;
	UCHAR	EnabledSize;
	UCHAR	ErrStatus;
} MemModuleInfo, * PMemModuleInfo;

typedef struct _TYPE_7_ {
	SMBIOSHEADER Header;
	UCHAR	SocketDesignation;
	UINT16	Configuration;
	UINT16	MaxSize;
	UINT16	InstalledSize;
	UINT16	SupportSRAMType;
	UINT16	CurrentSRAMType;
	UCHAR	Speed;
	UCHAR	ErrorCorrectionType;
	UCHAR	SystemCacheType;
	UCHAR	Associativity;
	DWORD   MaxSize2;
	DWORD   InstalledSize2;
} CacheInfo, * PCacheInfo;

typedef struct _TYPE_16_ {
	SMBIOSHEADER Header;
	UCHAR	Location;
	UCHAR	Use;
	UCHAR	ErrCorrection;
	UINT32	MaxCapacity;
	UINT16	ErrInfoHandle;
	UINT16	NumOfMDs;
	UINT64	ExtMaxCapacity;
} MemoryArray, * PMemoryArray;

typedef struct _TYPE_17_ {
	SMBIOSHEADER Header;
	UINT16	PhysicalArrayHandle;
	UINT16	ErrorInformationHandle;
	UINT16	TotalWidth;
	UINT16	DataWidth;
	UINT16	Size;
	UCHAR	FormFactor;
	UCHAR	DeviceSet;
	UCHAR	DeviceLocator;
	UCHAR	BankLocator;
	UCHAR	MemoryType;
	UINT16	TypeDetail;
	UINT16	Speed;
	UCHAR	Manufacturer;
	UCHAR	SN;
	UCHAR	AssetTag;
	UCHAR	PN;
	UCHAR	Attributes;
} MemoryDevice, * PMemoryDevice;

typedef struct _TYPE_19_ {
	SMBIOSHEADER Header;
	ULONG32	Starting;
	ULONG32	Ending;
	UINT16	Handle;
	UCHAR	PartitionWidth;
} MemoryArrayMappedAddress, * PMemoryArrayMappedAddress;

typedef struct _TYPE_22_ {
	SMBIOSHEADER Header;
	UCHAR	Location;
	UCHAR	Manufacturer;
	UCHAR	Date;
	UCHAR	SN;
	UCHAR	DeviceName;
	UCHAR   DeviceChemistry;
	UINT16  DesignCapacity;
	UINT16  DesignVoltage;

} PortableBattery, * PPortableBattery;

typedef struct _TYPE_43_ {
	SMBIOSHEADER Header;
	UCHAR   Vendor[4];
	UCHAR   MajorSpecVer;
	UCHAR   MinorSpecVer;
	DWORD   FwVer1;
	DWORD   FwVer2;
	UCHAR   Description;
	UINT64  Characteristics;
	DWORD   OEM;
} TPMDevice, * PTMPDevice;

struct mbr_entry
{
	/* If active, 0x80, otherwise, 0x00.  */
	uint8_t flag;
	/* The head of the start.  */
	uint8_t start_head;
	/* (S | ((C >> 2) & 0xC0)) where S is the sector of the start and C
	   is the cylinder of the start. Note that S is counted from one.  */
	uint8_t start_sector;
	/* (C & 0xFF) where C is the cylinder of the start.  */
	uint8_t start_cylinder;
	/* The partition type.  */
	uint8_t type;
	/* The end versions of start_head, start_sector and start_cylinder,
	   respectively.  */
	uint8_t end_head;
	uint8_t end_sector;
	uint8_t end_cylinder;
	/* The start sector. Note that this is counted from zero.  */
	uint32_t start;
	/* The length in sector units.  */
	uint32_t length;
};

struct mbr_header
{
	char dummy1[11];/* normally there is a short JMP instuction(opcode is 0xEB) */
	uint16_t bytes_per_sector;/* seems always to be 512, so we just use 512 */
	uint8_t sectors_per_cluster;/* non-zero, the power of 2, i.e., 2^n */
	uint16_t reserved_sectors;/* FAT=non-zero, NTFS=0? */
	uint8_t number_of_fats;/* NTFS=0; FAT=1 or 2  */
	uint16_t root_dir_entries;/* FAT32=0, NTFS=0, FAT12/16=non-zero */
	uint16_t total_sectors_short;/* FAT32=0, NTFS=0, FAT12/16=any */
	uint8_t media_descriptor;/* range from 0xf0 to 0xff */
	uint16_t sectors_per_fat;/* FAT32=0, NTFS=0, FAT12/16=non-zero */
	uint16_t sectors_per_track;/* range from 1 to 63 */
	uint16_t total_heads;/* range from 1 to 256 */
	uint32_t hidden_sectors;/* any value */
	uint32_t total_sectors_long;/* FAT32=non-zero, NTFS=0, FAT12/16=any */
	uint32_t sectors_per_fat32;/* FAT32=non-zero, NTFS=any, FAT12/16=any */
	uint64_t total_sectors_long_long;/* NTFS=non-zero, FAT12/16/32=any */
	char dummy2[392];
	uint8_t unique_signature[4];
	uint8_t unknown[2];

	/* Four partition entries.  */
	struct mbr_entry entries[4];

	/* The signature 0xaa55.  */
	uint16_t signature;
};

struct gpt_header
{
	uint8_t magic[8];
	uint32_t version;
	uint32_t headersize;
	uint32_t crc32;
	uint32_t unused1;
	uint64_t header_lba;
	uint64_t alternate_lba;
	uint64_t start;
	uint64_t end;
	uint8_t guid[16];
	uint64_t partitions;
	uint32_t maxpart;
	uint32_t partentry_size;
	uint32_t partentry_crc32;
};

typedef struct PHY_DRIVE_INFO
{
	//int Id;
	int PhyDrive;
	int PartStyle;//0:UNKNOWN 1:MBR 2:GPT
	UINT64 SizeInBytes;
	BYTE DeviceType;
	BOOL RemovableMedia;
	CHAR VendorId[128];
	CHAR ProductId[128];
	CHAR ProductRev[128];
	CHAR SerialNumber[128];
	STORAGE_BUS_TYPE BusType;
	// MBR
	UCHAR MbrSignature[4];
	// GPT
	UCHAR GptGuid[16];

	CHAR DriveLetters[26];
}PHY_DRIVE_INFO;

#pragma pack()

