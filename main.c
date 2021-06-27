#include <Uefi.h>

#include "uefi_interface.h"

EFI_STATUS
EFIAPI
_ModuleEntryPoint (
	IN EFI_HANDLE ImageHandle,
	IN EFI_SYSTEM_TABLE *st
) {
	UINTN index;

	uefi_initialize(ImageHandle, st);
	fill_background(EFI_BACKGROUND_RED);

	st->ConOut->SetCursorPosition(st->ConOut, 0, 0);
	blt_test();

 	st->BootServices->WaitForEvent(1, &st->ConIn->WaitForKey, &index);

	return EFI_SUCCESS;
}
