#include <Uefi.h>
#include <Protocol/GraphicsOutput.h>

#include "common.h"
#include "printf.h"

EFI_HANDLE         gImageHandle = NULL;
EFI_SYSTEM_TABLE   *gst         = NULL;
EFI_BOOT_SERVICES  *gbs         = NULL;

EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL	*gco = NULL;
EFI_SIMPLE_TEXT_INPUT_PROTOCOL  *gci = NULL;

uintn_t rows, cols;

int uefi_initialize (EFI_HANDLE handle, EFI_SYSTEM_TABLE *st)
{
	gImageHandle = handle;
	gst = st;
	gbs = st->BootServices;

	gco = st->ConOut;
	gci = st->ConIn;

	gco->QueryMode(gco, gco->Mode->Mode, &cols, &rows);

	return 0;
}

void *malloc (uintn_t size)
{
	void *ptr = NULL;

	EFI_STATUS result = gbs->AllocatePool(EfiLoaderData, size, &ptr);
	return result == EFI_SUCCESS ? ptr : NULL;
}

void free (void *ptr)
{
	gbs->FreePool(ptr);
}

void putchar (wchar_t c)
{
	wchar_t s[] = { c, 0 };
	gco->OutputString(gco, s);
}

void print (wchar_t *str)
{
	gco->OutputString(gco, str);
}

void println (wchar_t *line)
{
	print(line);
	gco->OutputString(gco, L"\r\n");
}

void fill_background (uintn_t color)
{
	gco->SetAttribute(gco, color);
	for (int i = 0; i < rows; ++i) {
		gco->SetCursorPosition(gco, 0, i);
		for (int j = 0; j < cols; ++j)
			gco->OutputString(gco, L" ");
	}
}

void checker_background (uintn_t color1, uintn_t color2)
{
	fill_background(color1);

	gco->SetAttribute(gco, color2);
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			if ((i + j) % 2) {
				gco->SetCursorPosition(gco, j, i);
				gco->OutputString(gco, L" ");
			}
		}
	}
}

void draw_border (uintn_t color)
{
	gco->EnableCursor(gco, FALSE);
	gco->SetAttribute(gco, color);

	gco->SetCursorPosition(gco, 0, 1);
	gco->OutputString(gco, L"┌");
	gco->SetCursorPosition(gco, cols - 1, 1);
	gco->OutputString(gco, L"┐");
	gco->SetCursorPosition(gco, 0, rows - 1);
	gco->OutputString(gco, L"└");
	gco->SetCursorPosition(gco, cols - 1, rows - 1);
	gco->OutputString(gco, L"┘\r");

	gco->SetCursorPosition(gco, 1, 1);
}

typedef EFI_GRAPHICS_OUTPUT_BLT_PIXEL pxel;

void blt_test ()
{
	EFI_STATUS result;

	EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
	EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;

	result = gbs->LocateProtocol(&gopGuid, NULL, (void**) &gop);
	if (result != EFI_SUCCESS) {
		println(L"no protocol instances found");
		return;
	}

	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
	uintn_t size_info, mode_count, mode_native;

	result = gop->QueryMode(gop,
		gop->Mode == NULL ? 0 : gop->Mode->Mode,
		&size_info, &info);
	if (result != EFI_SUCCESS)
		result = gop->SetMode(gop, 0);
	if (result != EFI_SUCCESS)
		println(L"unable to query native mode");
	else {
		mode_native = gop->Mode->Mode;
		mode_count = gop->Mode->MaxMode;
	}

	printf(L"mode_native = %d\r\n", (intn_t) mode_native);
	printf(L"mode_count  = %d\r\n", (intn_t) mode_count);

	for (intn_t i = 0; i < mode_count - 10; ++i) {
		result = gop->QueryMode(gop, (UINT32) i, &size_info, &info);
		if (result == EFI_SUCCESS)
			printf(L"mode %d width %d height %d format %d %s\r\n",
				(intn_t) i,
				(intn_t) info->HorizontalResolution,
				(intn_t) info->VerticalResolution,
				(intn_t) info->PixelFormat,
				i == mode_native ? L"(current)" : L"");
	}

	result = gop->SetMode(gop, gop->Mode->Mode);

	uint32_t
		black  = 0x000000,
		white  = 0xffffff,
		red    = 0xff0000,
		green  = 0x00ff00,
		blue   = 0x0000ff,
		orange = 0xffa500;

	uint32_t h = gop->Mode->Info->VerticalResolution,
		w = gop->Mode->Info->HorizontalResolution;
	uint32_t m = w / 2, sq = h / 5;
	uint32_t xls = (m - sq) / 2;

	gop->Blt(gop, (pxel*) &black,  EfiBltVideoFill, 0, 0, 0, 0, m, h, 0);
	gop->Blt(gop, (pxel*) &white,  EfiBltVideoFill, 0, 0, m, 0, m, h, 0);
	gop->Blt(gop, (pxel*) &green,  EfiBltVideoFill, 0, 0, xls, sq, sq, sq, 0);
	gop->Blt(gop, (pxel*) &red,    EfiBltVideoFill, 0, 0, xls, 3*sq, sq, sq, 0);
	gop->Blt(gop, (pxel*) &orange, EfiBltVideoFill, 0, 0, m + xls, sq, sq, sq, 0);
	gop->Blt(gop, (pxel*) &blue,   EfiBltVideoFill, 0, 0, m + xls, 3*sq, sq, sq, 0);
}
