// SPDX-License-Identifier: Unlicense

#include "gnwinfo.h"

static LPCSTR
get_node_attr(PNODE node, LPCSTR key)
{
	LPCSTR str;
	if (!node)
		return "-";
	str = NWL_NodeAttrGet(node, key);
	if (!str)
		return "-";
	return str;
}

static LPCSTR
get_smbios_attr(LPCSTR type, LPCSTR key, BOOL(*cond)(PNODE node))
{
	INT i, count;
	count = NWL_NodeChildCount(g_ctx.smbios);
	for (i = 0; i < count; i++)
	{
		LPCSTR attr;
		PNODE tab = g_ctx.smbios->Children[i].LinkedNode;
		attr = get_node_attr(tab, "Table Type");
		if (!attr || strcmp(attr, type) != 0)
			continue;
		if (!cond || cond(tab) == TRUE)
			return get_node_attr(tab, key);
	}
	return "-";
}

#define MAIN_GUI_ROW_2_BEGIN \
	nk_layout_row_begin(ctx, NK_DYNAMIC, 0, 2); \
	nk_layout_row_push(ctx, 0.40f);

#define MAIN_GUI_ROW_2_MID1 \
	nk_layout_row_push(ctx, 0.60f);

#define MAIN_GUI_ROW_2_END \
	nk_layout_row_end(ctx);

#define MAIN_GUI_ROW_3_BEGIN \
	nk_layout_row_begin(ctx, NK_DYNAMIC, 0, 3); \
	nk_layout_row_push(ctx, 0.20f);

#define MAIN_GUI_ROW_3_MID1 \
	nk_layout_row_push(ctx, 0.20f);

#define MAIN_GUI_ROW_3_MID2 \
	nk_layout_row_push(ctx, 0.60f);

#define MAIN_GUI_ROW_3_END \
	nk_layout_row_end(ctx);

#define MAIN_GUI_LABEL(title,icon) \
{ \
	nk_layout_row_begin(ctx, NK_DYNAMIC, 0, 4); \
	struct nk_rect _rect = nk_layout_widget_bounds(ctx); \
	nk_layout_row_push(ctx, _rect.h / _rect.w); \
	nk_image(ctx, icon); \
	nk_layout_row_push(ctx, 1.0f - _rect.h / _rect.w); \
	nk_label(ctx, title, NK_TEXT_LEFT); \
	nk_layout_row_end(ctx); \
}

static VOID
draw_os(struct nk_context* ctx)
{
	MAIN_GUI_LABEL("Operating System", g_ctx.image_os);
	MAIN_GUI_ROW_2_BEGIN;
	nk_label(ctx, "    Name", NK_TEXT_LEFT);
	MAIN_GUI_ROW_2_MID1;
	nk_labelf_colored(ctx, NK_TEXT_LEFT,
		nk_rgb(255, 255, 255),
		"%s %s (%s)",
		get_node_attr(g_ctx.system, "OS"),
		get_node_attr(g_ctx.system, "Processor Architecture"),
		get_node_attr(g_ctx.system, "Build Number"));
	MAIN_GUI_ROW_2_END;
	MAIN_GUI_ROW_2_BEGIN;
	nk_spacer(ctx);
	MAIN_GUI_ROW_2_MID1;
	nk_labelf_colored(ctx, NK_TEXT_LEFT,
		nk_rgb(255, 255, 255),
		"%s@%s%s%s",
		get_node_attr(g_ctx.system, "Username"),
		get_node_attr(g_ctx.system, "Computer Name"),
		strcmp(get_node_attr(g_ctx.system, "Safe Mode"), "Yes") == 0 ? " SafeMode" : "",
		strcmp(get_node_attr(g_ctx.system, "BitLocker Boot"), "Yes") == 0 ? " BitLocker" : "");
	MAIN_GUI_ROW_2_END;
	MAIN_GUI_ROW_2_BEGIN;
	nk_label(ctx, "    Uptime", NK_TEXT_LEFT);
	MAIN_GUI_ROW_2_MID1;
	nk_label_colored(ctx,
		NWL_GetUptime(),
		NK_TEXT_LEFT,
		nk_rgb(255, 255, 255));
	MAIN_GUI_ROW_2_END;
}

static VOID
draw_bios(struct nk_context* ctx)
{
	LPCSTR tpm = get_node_attr(g_ctx.system, "TPM");
	MAIN_GUI_LABEL("BIOS", g_ctx.image_bios);
	MAIN_GUI_ROW_2_BEGIN;
	nk_label(ctx, "    Firmware", NK_TEXT_LEFT);
	MAIN_GUI_ROW_2_MID1;
	nk_labelf_colored(ctx, NK_TEXT_LEFT,
		nk_rgb(255, 255, 255),
		"%s%s%s%s",
		get_node_attr(g_ctx.uefi, "Firmware Type"),
		strcmp(get_node_attr(g_ctx.uefi, "Secure Boot"), "ENABLED") == 0 ? " Secure Boot" : "",
		tpm[0] == 'v' ? " TPM" : "",
		tpm[0] == 'v' ? tpm : "");
	MAIN_GUI_ROW_2_END;
	MAIN_GUI_ROW_2_BEGIN;
	nk_label(ctx, "    Version", NK_TEXT_LEFT);
	MAIN_GUI_ROW_2_MID1;
	nk_labelf_colored(ctx, NK_TEXT_LEFT,
		nk_rgb(255, 255, 255),
		"%s %s",
		get_smbios_attr("0", "Vendor", NULL),
		get_smbios_attr("0", "Version", NULL));
	MAIN_GUI_ROW_2_END;
}

static BOOL
is_motherboard(PNODE node)
{
	LPCSTR str = get_node_attr(node, "Board Type");
	return (strcmp(str, "Motherboard") == 0);
}

static VOID
draw_motherboard(struct nk_context* ctx)
{
	MAIN_GUI_LABEL("Motherboard", g_ctx.image_board);
	MAIN_GUI_ROW_2_BEGIN;
	nk_label(ctx, "    Product Name", NK_TEXT_LEFT);
	MAIN_GUI_ROW_2_MID1;
	nk_labelf_colored(ctx, NK_TEXT_LEFT,
		nk_rgb(255, 255, 255),
		"%s %s",
		get_smbios_attr("2", "Manufacturer", is_motherboard),
		get_smbios_attr("2", "Product Name", is_motherboard));
	MAIN_GUI_ROW_2_END;
	MAIN_GUI_ROW_2_BEGIN;
	nk_label(ctx, "    Serial Number", NK_TEXT_LEFT);
	MAIN_GUI_ROW_2_MID1;
	nk_label_colored(ctx,
		get_smbios_attr("2", "Serial Number", is_motherboard),
		NK_TEXT_LEFT,
		nk_rgb(255, 255, 255));
	MAIN_GUI_ROW_2_END;
}

static uint8_t cache_level = 0;
static BOOL
is_cache_level_equal(PNODE node)
{
	LPCSTR str = get_node_attr(node, "Cache Level");
	CHAR buf[] = "L1";
	buf[1] = '0' + cache_level;
	return (strcmp(str, buf) == 0);
}

static VOID
draw_processor(struct nk_context* ctx)
{
	INT i, j, count;
	MAIN_GUI_LABEL("Processor", g_ctx.image_cpu);
	count = NWL_NodeChildCount(g_ctx.smbios);
	for (i = 0, j = 0; i < count; i++)
	{
		LPCSTR attr;
		PNODE tab = g_ctx.smbios->Children[i].LinkedNode;
		attr = get_node_attr(tab, "Table Type");
		if (strcmp(attr, "4") != 0)
			continue;
		MAIN_GUI_ROW_2_BEGIN;
		nk_labelf(ctx, NK_TEXT_LEFT, "    CPU%d", j++);
		MAIN_GUI_ROW_2_MID1;
		nk_label_colored(ctx,
			get_node_attr(tab, "Processor Version"),
			NK_TEXT_LEFT,
			nk_rgb(255, 255, 255));
		MAIN_GUI_ROW_2_END;
		MAIN_GUI_ROW_2_BEGIN;
		nk_spacer(ctx);
		MAIN_GUI_ROW_2_MID1;
		nk_labelf_colored(ctx, NK_TEXT_LEFT, nk_rgb(255, 255, 255),
			"%s %s cores %s threads",
			get_node_attr(tab, "Socket Designation"),
			get_node_attr(tab, "Core Count"),
			get_node_attr(tab, "Thread Count"));
		MAIN_GUI_ROW_2_END;
	}

	LPCSTR cache_size[4];
	for (cache_level = 1; cache_level <= 4; cache_level++)
		cache_size[cache_level - 1] = get_smbios_attr("7", "Installed Cache Size", is_cache_level_equal);
	MAIN_GUI_ROW_2_BEGIN;
	nk_label(ctx, "    Cache Size", NK_TEXT_LEFT);
	MAIN_GUI_ROW_2_MID1;
	if (cache_size[0][0] == '-')
		nk_label_colored(ctx, cache_size[0], NK_TEXT_LEFT, nk_rgb(255, 255, 255));
	else if (cache_size[1][0] == '-')
		nk_labelf_colored(ctx, NK_TEXT_LEFT, nk_rgb(255, 255, 255),
			"L1 %s", cache_size[0]);
	else if (cache_size[2][0] == '-')
		nk_labelf_colored(ctx, NK_TEXT_LEFT, nk_rgb(255, 255, 255),
			"L1 %s L2 %s", cache_size[0], cache_size[1]);
	else if (cache_size[3][0] == '-')
		nk_labelf_colored(ctx, NK_TEXT_LEFT, nk_rgb(255, 255, 255),
			"L1 %s L2 %s L3 %s", cache_size[0], cache_size[1], cache_size[2]);
	else
		nk_labelf_colored(ctx, NK_TEXT_LEFT, nk_rgb(255, 255, 255),
			"L1 %s L2 %s L3 %s L4 %s", cache_size[0], cache_size[1], cache_size[2], cache_size[3]);
	MAIN_GUI_ROW_2_END;
}

LPCSTR NWL_GetHumanSize(UINT64 size, LPCSTR human_sizes[6], UINT64 base);
static const char* mem_human_sizes[6] =
{ "B", "K", "M", "G", "T", "P", };

static VOID
draw_memory(struct nk_context* ctx)
{
	INT i, count;
	struct nk_color color = nk_rgb(0, 255, 0); // Green
	MEMORYSTATUSEX statex = { 0 };
	char buf[48];
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);
	strcpy_s(buf, sizeof(buf), NWL_GetHumanSize(statex.ullAvailPhys, mem_human_sizes, 1024));
	if (statex.dwMemoryLoad > 60)
		color = nk_rgb(255, 255, 0); // Yellow
	if (statex.dwMemoryLoad > 80)
		color = nk_rgb(255, 0, 0); // Red
	MAIN_GUI_LABEL("Memory", g_ctx.image_ram);
	MAIN_GUI_ROW_2_BEGIN;
	nk_label(ctx, "    Usage", NK_TEXT_LEFT);
	MAIN_GUI_ROW_2_MID1;
	nk_labelf_colored(ctx, NK_TEXT_LEFT, color,
		"%lu%% %s / %s",
		statex.dwMemoryLoad, buf,
		NWL_GetHumanSize(statex.ullTotalPhys, mem_human_sizes, 1024));
	MAIN_GUI_ROW_2_END;
	count = NWL_NodeChildCount(g_ctx.smbios);
	for (i = 0; i < count; i++)
	{
		LPCSTR attr, ddr;
		PNODE tab = g_ctx.smbios->Children[i].LinkedNode;
		attr = get_node_attr(tab, "Table Type");
		if (strcmp(attr, "17") != 0)
			continue;
		ddr = get_node_attr(tab, "Device Type");
		if (ddr[0] == '-')
			continue;
		MAIN_GUI_ROW_2_BEGIN;
		nk_labelf(ctx, NK_TEXT_LEFT, "    %s", get_node_attr(tab, "Bank Locator"));
		MAIN_GUI_ROW_2_MID1;
		nk_labelf_colored(ctx, NK_TEXT_LEFT,
			nk_rgb(255, 255, 255),
			"%s-%s %s %s %s",
			ddr,
			get_node_attr(tab, "Speed (MT/s)"),
			get_node_attr(tab, "Device Size"),
			get_node_attr(tab, "Manufacturer"),
			get_node_attr(tab, "Serial Number"));
		MAIN_GUI_ROW_2_END;
	}
}

static VOID
draw_monitor(struct nk_context* ctx)
{
	INT i, count;
	MAIN_GUI_LABEL("Display Devices", g_ctx.image_edid);
	count = NWL_NodeChildCount(g_ctx.edid);
	for (i = 0; i < count; i++)
	{
		PNODE mon = g_ctx.edid->Children[i].LinkedNode;
		PNODE res, sz;
		if (!mon)
			continue;
		res = NWL_NodeGetChild(mon, "Resolution");
		sz = NWL_NodeGetChild(mon, "Screen Size");
		MAIN_GUI_ROW_2_BEGIN;
		nk_labelf(ctx, NK_TEXT_LEFT, "    %s",
			get_node_attr(mon, "HWID"));
		MAIN_GUI_ROW_2_MID1;
		nk_labelf_colored(ctx, NK_TEXT_LEFT,
			nk_rgb(255, 255, 255),
			"%sx%s@%sHz %s\"",
			get_node_attr(res, "Width"),
			get_node_attr(res, "Height"),
			get_node_attr(res, "Refresh Rate (Hz)"),
			get_node_attr(sz, "Diagonal (in)"));
		MAIN_GUI_ROW_2_END;
	}
}

static VOID
draw_storage(struct nk_context* ctx)
{
	INT i, count;
	MAIN_GUI_LABEL("Storage", g_ctx.image_disk);
	count = NWL_NodeChildCount(g_ctx.disk);
	for (i = 0; i < count; i++)
	{
		BOOL cdrom;
		LPCSTR path, id, type;
		PNODE disk = g_ctx.disk->Children[i].LinkedNode;
		if (!disk)
			continue;
		path = get_node_attr(disk, "Path");
		if (strncmp(path, "\\\\.\\CdRom", 9) == 0)
		{
			cdrom = TRUE;
			id = &path[9];
			type = "CDROM";
		}
		else if (strncmp(path, "\\\\.\\PhysicalDrive", 17) == 0)
		{
			cdrom = FALSE;
			id = &path[17];
			type = strcmp(get_node_attr(disk, "SSD"), "Yes") == 0 ? "SSD" : "HDD";
		}
		else
			continue;
		MAIN_GUI_ROW_3_BEGIN;
		nk_labelf(ctx, NK_TEXT_LEFT,
			"    %s%s",
			cdrom ? "CDROM" : "DISK",
			id);
		MAIN_GUI_ROW_3_MID1;
		nk_label(ctx, "Product Name", NK_TEXT_LEFT);
		MAIN_GUI_ROW_3_MID2;
		nk_labelf_colored(ctx, NK_TEXT_LEFT,
			nk_rgb(255, 255, 255),
			"%s (%s)",
			get_node_attr(disk, "Product ID"),
			get_node_attr(disk, "Product Rev"));
		MAIN_GUI_ROW_3_END;
		MAIN_GUI_ROW_3_BEGIN;
		nk_spacer(ctx);
		MAIN_GUI_ROW_3_MID1;
		nk_label(ctx, "Info", NK_TEXT_LEFT);
		MAIN_GUI_ROW_3_MID2;
		nk_labelf_colored(ctx, NK_TEXT_LEFT,
			nk_rgb(255, 255, 255),
			"%s %s %s %s",
			get_node_attr(disk, "Size"),
			get_node_attr(disk, "Type"),
			type,
			get_node_attr(disk, "Partition Table"));
		MAIN_GUI_ROW_3_END;
		LPCSTR health = get_node_attr(disk, "Health Status");
		LPCSTR temp = get_node_attr(disk, "Temperature (C)");
		struct nk_color color = nk_rgb(255, 255, 0); // Yellow
		if (strcmp(health, "-") == 0)
			continue;
		if (strncmp(health, "Good", 4) == 0)
			color = nk_rgb(0, 255, 0); // Green
		else if (strncmp(health, "Bad", 3) == 0)
			color = nk_rgb(255, 0, 0); // Red
		MAIN_GUI_ROW_3_BEGIN;
		nk_spacer(ctx);
		MAIN_GUI_ROW_3_MID1;
		nk_label(ctx, "S.M.A.R.T", NK_TEXT_LEFT);
		MAIN_GUI_ROW_3_MID2;
		nk_labelf_colored(ctx, NK_TEXT_LEFT,
			color, "%s %s%s", health,
			temp[0] != '-' ? temp : "",
			temp[0] != '-' ? "(C)" : "");
		MAIN_GUI_ROW_3_END;
	}
}

static LPCSTR
get_first_ipv4(PNODE node)
{
	INT i, count;
	PNODE unicasts = NWL_NodeGetChild(node, "Unicasts");
	if (!unicasts)
		return "";
	count = NWL_NodeChildCount(unicasts);
	for (i = 0; i < count; i++)
	{
		PNODE ip = unicasts->Children[i].LinkedNode;
		LPCSTR addr = get_node_attr(ip, "IPv4");
		if (strcmp(addr, "-") != 0)
			return addr;
	}
	return "";
}

static VOID
draw_network(struct nk_context* ctx)
{
	INT i, count;
	if (g_ctx.network)
		NWL_NodeFree(g_ctx.network, 1);
	g_ctx.network = NW_Network();
	MAIN_GUI_LABEL("Network", g_ctx.image_net);
	count = NWL_NodeChildCount(g_ctx.network);
	for (i = 0; i < count; i++)
	{
		PNODE nw = g_ctx.network->Children[i].LinkedNode;
		struct nk_color color = nk_rgb(255, 0, 0); // Red
		if (!nw)
			continue;
		nk_layout_row_begin(ctx, NK_DYNAMIC, 0, 2);
		nk_layout_row_push(ctx, 0.60f);
		nk_labelf(ctx, NK_TEXT_LEFT, "    %s", get_node_attr(nw, "Description"));
		nk_layout_row_push(ctx, 0.40f);
		if (strcmp(get_node_attr(nw, "Status"), "Active") == 0)
			color = nk_rgb(0, 255, 0); // Green
		nk_labelf_colored(ctx,
			NK_TEXT_LEFT, color,
			"%s%s",
			get_first_ipv4(nw),
			strcmp(get_node_attr(nw, "DHCP Enabled"), "Yes") == 0 ? " DHCP" : "");
		nk_layout_row_end(ctx);
	}
}

#define MAIN_INFO_OS        (1U << 0)
#define MAIN_INFO_BIOS      (1U << 1)
#define MAIN_INFO_BOARD     (1U << 2)
#define MAIN_INFO_CPU       (1U << 3)
#define MAIN_INFO_MEMORY    (1U << 4)
#define MAIN_INFO_MONITOR   (1U << 5)
#define MAIN_INFO_STORAGE   (1U << 6)
#define MAIN_INFO_NETWORK   (1U << 7)

VOID
gnwinfo_draw_main_window(struct nk_context* ctx, float width, float height)
{
	if (!nk_begin(ctx, "Summary",
		nk_rect(0, 0, width, height),
		NK_WINDOW_BACKGROUND))
		goto out;
	nk_layout_row_begin(ctx, NK_DYNAMIC, 20, 12);
	struct nk_rect rect = nk_layout_widget_bounds(ctx);
	float ratio = rect.h / rect.w;
	nk_layout_row_push(ctx, ratio);
	if (nk_button_image(ctx, g_ctx.image_os))
		g_ctx.main_flag ^= MAIN_INFO_OS;
	nk_layout_row_push(ctx, ratio);
	if (nk_button_image(ctx, g_ctx.image_bios))
		g_ctx.main_flag ^= MAIN_INFO_BIOS;
	nk_layout_row_push(ctx, ratio);
	if (nk_button_image(ctx, g_ctx.image_board))
		g_ctx.main_flag ^= MAIN_INFO_BOARD;
	nk_layout_row_push(ctx, ratio);
	if (nk_button_image(ctx, g_ctx.image_cpu))
		g_ctx.main_flag ^= MAIN_INFO_CPU;
	nk_layout_row_push(ctx, ratio);
	if (nk_button_image(ctx, g_ctx.image_ram))
		g_ctx.main_flag ^= MAIN_INFO_MEMORY;
	nk_layout_row_push(ctx, ratio);
	if (nk_button_image(ctx, g_ctx.image_edid))
		g_ctx.main_flag ^= MAIN_INFO_MONITOR;
	nk_layout_row_push(ctx, ratio);
	if (nk_button_image(ctx, g_ctx.image_disk))
		g_ctx.main_flag ^= MAIN_INFO_STORAGE;
	nk_layout_row_push(ctx, ratio);
	if (nk_button_image(ctx, g_ctx.image_net))
		g_ctx.main_flag ^= MAIN_INFO_NETWORK;
	nk_layout_row_push(ctx, 1.0f > 11.0f * ratio ? (1.0f - 11.0f * ratio) : 0);
	nk_spacer(ctx);
	nk_layout_row_push(ctx, ratio);
	if (nk_button_image(ctx, g_ctx.image_cpuid))
		MessageBoxA(g_ctx.wnd, "CPUID", "Error", MB_ICONERROR);
	nk_layout_row_push(ctx, ratio);
	if (nk_button_image(ctx, g_ctx.image_smart))
		MessageBoxA(g_ctx.wnd, "SMART", "Error", MB_ICONERROR);
	nk_layout_row_push(ctx, ratio);
	if (nk_button_image(ctx, g_ctx.image_close))
		gnwinfo_ctx_exit();
	nk_layout_row_end(ctx);

	if (g_ctx.main_flag & MAIN_INFO_OS)
		draw_os(ctx);
	if (g_ctx.main_flag & MAIN_INFO_BIOS)
		draw_bios(ctx);
	if (g_ctx.main_flag & MAIN_INFO_BOARD)
		draw_motherboard(ctx);
	if (g_ctx.main_flag & MAIN_INFO_CPU)
		draw_processor(ctx);
	if (g_ctx.main_flag & MAIN_INFO_MEMORY)
		draw_memory(ctx);
	if (g_ctx.main_flag & MAIN_INFO_MONITOR)
		draw_monitor(ctx);
	if (g_ctx.main_flag & MAIN_INFO_STORAGE)
		draw_storage(ctx);
	if (g_ctx.main_flag & MAIN_INFO_NETWORK)
		draw_network(ctx);

out:
	nk_end(ctx);
}