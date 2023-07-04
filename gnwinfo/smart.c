// SPDX-License-Identifier: Unlicense

#include "gnwinfo.h"
#include "../libcdi/libcdi.h"

LPCSTR NWL_GetHumanSize(UINT64 size, LPCSTR human_sizes[6], UINT64 base);
static const char* disk_human_sizes[6] =
{ "MB", "GB", "TB", "PB", "EB", "ZB", };

static void
draw_rect(struct nk_context* ctx, struct nk_color bg, const char* str)
{
	static struct nk_style_button style;
	memcpy(&style, &ctx->style.button, sizeof(struct nk_style_button));
	style.normal.type = NK_STYLE_ITEM_COLOR;
	style.normal.data.color = bg;
	style.hover.type = NK_STYLE_ITEM_COLOR;
	style.hover.data.color = bg;
	style.active.type = NK_STYLE_ITEM_COLOR;
	style.active.data.color = bg;
	style.text_normal = (struct nk_color)NK_COLOR_BLACK;
	style.text_hover = (struct nk_color)NK_COLOR_BLACK;
	style.text_active = (struct nk_color)NK_COLOR_BLACK;
	nk_button_label_styled(ctx, &style, str);
}

static struct nk_color
get_attr_color(DWORD status)
{
	switch (status)
	{
	case CDI_DISK_STATUS_GOOD:
		return g_color_good;
	case CDI_DISK_STATUS_CAUTION:
		return g_color_warning;
	case CDI_DISK_STATUS_BAD:
		return g_color_error;
	}
	return g_color_unknown;
}

static void
draw_health(struct nk_context* ctx, CDI_SMART* smart, int disk, float height)
{
	int n;
	char* str;
	char tmp[32];

	if (nk_group_begin(ctx, "SMART Health", 0))
	{
		struct nk_color color = g_color_warning;
		nk_layout_row_dynamic(ctx, height / 5.0f, 1);
		nk_label(ctx, "Health Status", NK_TEXT_CENTERED);
		n = cdi_get_int(smart, disk, CDI_INT_LIFE);
		str = cdi_get_string(smart, disk, CDI_STRING_DISK_STATUS);
		if (strncmp(str, "Good", 4) == 0)
			color = g_color_good;
		else if (strncmp(str, "Bad", 3) == 0)
			color = g_color_error;
		if (n >= 0)
			snprintf(tmp, sizeof(tmp), "%s\n%d%%", str, n);
		else
			snprintf(tmp, sizeof(tmp), "%s", str);
		draw_rect(ctx, color, tmp);
		cdi_free_string(str);
		nk_label(ctx, "Temperature", NK_TEXT_CENTERED);
		color = g_color_warning;
		int alarm = cdi_get_int(smart, disk, CDI_INT_TEMPERATURE_ALARM);
		if (alarm <= 0)
			alarm = 60;
		n = cdi_get_int(smart, disk, CDI_INT_TEMPERATURE);
		if (n > 0 && n < alarm)
			color = g_color_good;
		snprintf(tmp, sizeof(tmp), u8"%d \u00B0C", n);
		draw_rect(ctx, color, tmp);
		nk_group_end(ctx);
	}
}

static void
draw_info(struct nk_context* ctx, CDI_SMART* smart, int disk)
{
	INT n;
	DWORD d;
	char* str;
	char* tmp;
	if (nk_group_begin(ctx, "SMART Info", 0))
	{
		BOOL is_ssd = cdi_get_bool(smart, disk, CDI_BOOL_SSD);
		BOOL is_nvme = cdi_get_bool(smart, disk, CDI_BOOL_SSD_NVME);

		nk_layout_row_dynamic(ctx, 0, 1);
		nk_spacer(ctx);

		nk_layout_row(ctx, NK_DYNAMIC, 0, 4, (float[4]) { 0.2f, 0.4f, 0.24f, 0.16f });

		nk_label(ctx, "Firmware", NK_TEXT_LEFT);
		str = cdi_get_string(smart, disk, CDI_STRING_FIRMWARE);
		nk_label_colored(ctx, str, NK_TEXT_LEFT, g_color_text_l);
		cdi_free_string(str);
		if (is_ssd)
		{
			n = cdi_get_int(smart, disk, CDI_INT_HOST_READS);
			nk_label(ctx, "Total Reads", NK_TEXT_LEFT);
			if (n < 0)
				nk_label_colored(ctx, "-", NK_TEXT_RIGHT, g_color_text_l);
			else
				nk_labelf_colored(ctx, NK_TEXT_RIGHT, g_color_text_l, "%d G", n);
		}
		else
		{
			d = cdi_get_dword(smart, disk, CDI_DWORD_BUFFER_SIZE);
			nk_label(ctx, "Buffer Size", NK_TEXT_LEFT);
			if (d >= 10 * 1024 * 1024) // 10 MB
				nk_labelf_colored(ctx, NK_TEXT_RIGHT, g_color_text_l, "%lu M", d / 1024 / 1024);
			else if (d > 1024)
				nk_labelf_colored(ctx, NK_TEXT_RIGHT, g_color_text_l, "%lu K", d / 1024);
			else
				nk_labelf_colored(ctx, NK_TEXT_RIGHT, g_color_text_l, "%lu B", d);
		}

		nk_label(ctx, "S / N", NK_TEXT_LEFT);
		str = cdi_get_string(smart, disk, CDI_STRING_SN);
		nk_label_colored(ctx, str, NK_TEXT_LEFT, g_color_text_l);
		cdi_free_string(str);
		if (is_ssd)
		{
			n = cdi_get_int(smart, disk, CDI_INT_HOST_WRITES);
			nk_label(ctx, "Total Writes", NK_TEXT_LEFT);
			if (n < 0)
				nk_label_colored(ctx, "-", NK_TEXT_RIGHT, g_color_text_l);
			else
				nk_labelf_colored(ctx, NK_TEXT_RIGHT, g_color_text_l, "%d G", n);
		}
		else
		{
			nk_label(ctx, "-", NK_TEXT_CENTERED);
			nk_label_colored(ctx, "-", NK_TEXT_RIGHT, g_color_text_l);
		}

		nk_label(ctx, "Interface", NK_TEXT_LEFT);
		str = cdi_get_string(smart, disk, CDI_STRING_INTERFACE);
		nk_label_colored(ctx, str, NK_TEXT_LEFT, g_color_text_l);
		cdi_free_string(str);
		if (is_ssd && !is_nvme)
		{
			n = cdi_get_int(smart, disk, CDI_INT_NAND_WRITES);
			nk_label(ctx, "NAND Writes", NK_TEXT_LEFT);
			if (n < 0)
				nk_label_colored(ctx, "-", NK_TEXT_RIGHT, g_color_text_l);
			else
				nk_labelf_colored(ctx, NK_TEXT_RIGHT, g_color_text_l, "%d G", n);
		}
		else
		{
			nk_label(ctx, "RPM", NK_TEXT_LEFT);
			if (is_ssd)
				nk_label_colored(ctx, "(SSD)", NK_TEXT_RIGHT, g_color_text_l);
			else
			{
				d = cdi_get_dword(smart, disk, CDI_DWORD_ROTATION_RATE);
				nk_labelf_colored(ctx, NK_TEXT_RIGHT, g_color_text_l, "%lu", d);
			}
			
		}

		nk_label(ctx, "Mode", NK_TEXT_LEFT);
		str = cdi_get_string(smart, disk, CDI_STRING_TRANSFER_MODE_CUR);
		tmp = cdi_get_string(smart, disk, CDI_STRING_TRANSFER_MODE_MAX);
		nk_labelf_colored(ctx, NK_TEXT_LEFT, g_color_text_l, "%s|%s", str, tmp);
		cdi_free_string(str);
		cdi_free_string(tmp);
		nk_label(ctx, "Power On Count", NK_TEXT_LEFT);
		d = cdi_get_dword(smart, disk, CDI_DWORD_POWER_ON_COUNT);
		nk_labelf_colored(ctx, NK_TEXT_RIGHT, g_color_text_l, "%lu", d);

		nk_label(ctx, "Drive", NK_TEXT_LEFT);
		str = cdi_get_string(smart, disk, CDI_STRING_DRIVE_MAP);
		nk_label_colored(ctx, str, NK_TEXT_LEFT, g_color_text_l);
		cdi_free_string(str);
		nk_label(ctx, "Power On Hours", NK_TEXT_LEFT);
		n = cdi_get_int(smart, disk, CDI_INT_POWER_ON_HOURS);
		if (n < 0)
			nk_label_colored(ctx, "-", NK_TEXT_RIGHT, g_color_text_l);
		else
			nk_labelf_colored(ctx, NK_TEXT_RIGHT, g_color_text_l, "%d", n);

		nk_layout_row(ctx, NK_DYNAMIC, 0, 2, (float[2]) { 0.2f, 0.8f });

		nk_label(ctx, "Standard", NK_TEXT_LEFT);
		str = cdi_get_string(smart, disk, CDI_STRING_VERSION_MAJOR);
		nk_label_colored(ctx, str, NK_TEXT_LEFT, g_color_text_l);
		cdi_free_string(str);

		nk_label(ctx, "Features", NK_TEXT_LEFT);
		nk_labelf_colored(ctx, NK_TEXT_LEFT, g_color_text_l, "%s%s%s%s%s%s%s%s%s%s",
			cdi_get_bool(smart, disk, CDI_BOOL_SMART) ? "SMART " : "",
			cdi_get_bool(smart, disk, CDI_BOOL_AAM) ?  "AAM " : "",
			cdi_get_bool(smart, disk, CDI_BOOL_APM) ? "APM " : "",
			cdi_get_bool(smart, disk, CDI_BOOL_NCQ) ? "NCQ " : "",
			cdi_get_bool(smart, disk, CDI_BOOL_NV_CACHE) ? "NVCache " : "",
			cdi_get_bool(smart, disk, CDI_BOOL_DEVSLP) ? "DEVSLP " : "",
			cdi_get_bool(smart, disk, CDI_BOOL_STREAMING) ? "Streaming " : "",
			cdi_get_bool(smart, disk, CDI_BOOL_GPL) ? "GPL " : "",
			cdi_get_bool(smart, disk, CDI_BOOL_TRIM) ? "TRIM " : "",
			cdi_get_bool(smart, disk, CDI_BOOL_VOLATILE_WRITE_CACHE) ? "VolatileWriteCache " : "");

		nk_group_end(ctx);
	}
}

static char*
draw_alert_icon(struct nk_context* ctx, BYTE id, DWORD status, const char* format, char* value)
{
	static char hex[18];
	// RawValues(8)
	// RawValues(6)
	if (format[0] == 'R')
	{
		strcpy_s(hex, sizeof(hex), value);
		value[0] = '\0';
	}
	// Cur RawValues(8)
	else if (strncmp(format, "Cur R", 5) == 0)
	{
		strcpy_s(hex, sizeof(hex), &value[4]);
		value[4] = '\0';
	}
	// Cur Wor --- RawValues(6)
	else if (strncmp(format, "Cur Wor --- R", 13) == 0)
	{
		strcpy_s(hex, sizeof(hex), &value[8]);
		value[8] = '\0';
	}
	// Cur Wor Thr RawValues(7)
	// Cur Wor Thr RawValues(6)
	else
	{
		strcpy_s(hex, sizeof(hex), &value[12]);
		value[12] = '\0';
	}

	draw_rect(ctx, get_attr_color(status), "");
	return hex;
}

static void
draw_smart(struct nk_context* ctx, CDI_SMART* smart, int disk)
{
	char* format;
	char* value;
	char* name;
	char* hex;
	if (nk_group_begin(ctx, "SMART Attr", NK_WINDOW_BORDER))
	{
		CDI_SMART_ATTRIBUTE* attr = cdi_get_smart_attribute(smart, disk);
		CDI_SMART_STATUS* status = cdi_get_smart_status(NWLC->NwSmart, disk);
		DWORD i, count = cdi_get_dword(smart, disk, CDI_DWORD_ATTR_COUNT);
		nk_layout_row(ctx, NK_DYNAMIC, 0, 4, (float[4]) { 0.05f, 0.05f, 0.55f, 0.35f });
		nk_spacer(ctx);
		nk_label(ctx, "ID", NK_TEXT_LEFT);
		nk_label(ctx, "Attribute", NK_TEXT_LEFT);
		format = cdi_get_smart_attribute_format(smart, disk);
		nk_label(ctx, format, NK_TEXT_LEFT);

		for (i = 0; i < count; i++)
		{
			if (attr[i].Id == 0)
				continue;
			name = cdi_get_smart_attribute_name(smart, disk, attr[i].Id);
			value = cdi_get_smart_attribute_value(smart, disk, i);
			hex = draw_alert_icon(ctx, attr[i].Id, status->Status[i], format, value);
			nk_labelf_colored(ctx, NK_TEXT_LEFT, g_color_text_l, "%02X", attr[i].Id);
			nk_label_colored(ctx, name, NK_TEXT_LEFT, g_color_text_l);
			if (g_ctx.smart_hex)
				nk_labelf_colored(ctx, NK_TEXT_LEFT, g_color_text_l,
					"%s%s", value, hex);
			else
				nk_labelf_colored(ctx, NK_TEXT_LEFT, g_color_text_l,
					"%s%llu", value, strtoull(hex, NULL, 16));
			cdi_free_string(name);
			cdi_free_string(value);
		}

		cdi_free_string(format);
		nk_group_end(ctx);
	}
}

VOID
gnwinfo_draw_smart_window(struct nk_context* ctx, float width, float height)
{
	INT count;
	char* str;
	static int cur_disk = 0;

	if (g_ctx.gui_smart == FALSE)
		return;
	if (!nk_begin(ctx, "S.M.A.R.T.",
		nk_rect(0, height / 6.0f, width * 0.98f, height / 1.5f),
		NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_CLOSABLE))
	{
		g_ctx.gui_smart = FALSE;
		goto out;
	}

	count = cdi_get_disk_count(NWLC->NwSmart);
	if (count <= 0)
	{
		nk_layout_row_dynamic(ctx, 0, 1);
		nk_label(ctx, "No disks found", NK_TEXT_CENTERED);
		goto out;
	}
	nk_layout_row(ctx, NK_DYNAMIC, 0, 4, (float[4]) { 0.12f, 0.6f, 0.2f, 0.08f });
	nk_property_int(ctx, "#", 0, &cur_disk, count - 1, 1, 1);
	str = cdi_get_string(NWLC->NwSmart, cur_disk, CDI_STRING_MODEL);
	nk_labelf_colored(ctx, NK_TEXT_CENTERED, g_color_text_l,
		"%s %s",
		str,
		NWL_GetHumanSize(cdi_get_dword(NWLC->NwSmart, cur_disk, CDI_DWORD_DISK_SIZE), disk_human_sizes, 1000));
	cdi_free_string(str);
	if (nk_button_image_label(ctx, g_ctx.image_refresh, "Refresh", NK_TEXT_CENTERED))
		cdi_update_smart(NWLC->NwSmart, cur_disk);
	g_ctx.smart_hex = !nk_check_label(ctx, "HEX", !g_ctx.smart_hex);
	
	nk_layout_row(ctx, NK_DYNAMIC, height / 4.0f, 2, (float[2]) {0.2f, 0.8f});
	draw_health(ctx, NWLC->NwSmart, cur_disk, height / 4.0f);
	draw_info(ctx, NWLC->NwSmart, cur_disk);

	nk_layout_row_dynamic(ctx, height / 3.0f, 1);
	draw_smart(ctx, NWLC->NwSmart, cur_disk);

out:
	nk_end(ctx);
}
