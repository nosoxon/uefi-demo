#include <Uefi.h>

#include "common.h"
#include "uefi_interface.h"

#ifndef __PRINTF_H
#define __PRINTF_H

void EFIAPI printf (const wchar_t *format, ...);

#endif /* __PRINTF_H */