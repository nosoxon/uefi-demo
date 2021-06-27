#include <Uefi.h>

#include "common.h"
#include "uefi_interface.h"


int isalpha (wchar_t c)
{
	return (c >= L'a' && c <= L'z') || (c >= L'A' && c <= L'Z');
}

uintn_t strlen (wchar_t *s) {
	uintn_t len = 0;
	while (*s++) ++len;
	return len;
}

void reverse (wchar_t *s)
{
	wchar_t c;
	for (uintn_t i = 0, j = strlen(s) - 1; i < j; ++i, --j) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}

wchar_t *itoa (intn_t n)
{
	wchar_t *str = malloc(20);
	if (str == NULL)
		return NULL;

	intn_t sign = n < 0;
	if (sign) n = -n;

	wchar_t *s = str; do {
		*s++ = n % 10 + L'0';
	} while (n /= 10);

	if (sign) *s++ = L'-';
	*s++ = L'\0';

	reverse(str);
	return str;
}

void EFIAPI printf (const wchar_t *format, ...)
{
	VA_LIST ap;
	VA_START(ap, format);

	for (const wchar_t *c = format; *c; ++c) {
		if (*c != L'%') {
			putchar(*c);
			continue;
		}

		switch (*++c) {
		case L'%':
			putchar(L'%');
			break;
		case L's':
			wchar_t *s = VA_ARG(ap, wchar_t*);
			print(s);
			break;
		case L'd':
			wchar_t *dec = itoa(VA_ARG(ap, intn_t));
			print(dec);
			free(dec);
			break;
		default:
			putchar(L'%');
			putchar(*c);
		}
	}

	VA_END(ap);
}

void snprintf (wchar_t *str, uintn_t size, const wchar_t *format, ...)
{
	VA_LIST ap;
	VA_START(ap, format);

	// unimplemented

	VA_END(ap);
}