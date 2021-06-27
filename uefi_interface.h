#include <Uefi.h>

#include "common.h"

#ifndef __UEFI_INTERFACE_H
#define __UEFI_INTERFACE_H

extern EFI_HANDLE         gImageHandle;
extern EFI_SYSTEM_TABLE   *gst;
extern EFI_BOOT_SERVICES  *gbs;

int uefi_initialize (EFI_HANDLE handle, EFI_SYSTEM_TABLE *st);

void putchar (wchar_t c);
void print (wchar_t *str);
void println (wchar_t *line);

void fill_background (uintn_t color);
void checker_background (uintn_t color1, uintn_t color2);
void draw_border (uintn_t color);

void *malloc (uintn_t size);
void free (void *ptr);

void blt_test ();

#endif
