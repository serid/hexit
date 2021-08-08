#include "Util.h"

#include <Windows.h>
#include <stdexcept>

using namespace std;

wstring open_file_dialog()
{
	const int count = 256;

	wchar_t* output = new wchar_t[count];
	output[0] = '\0';
	output[1] = '\0';

	OPENFILENAMEW open_file_name;
	open_file_name.lStructSize = sizeof open_file_name;
	open_file_name.hwndOwner = nullptr;
	open_file_name.hInstance = nullptr;
	open_file_name.lpstrFilter = nullptr;
	open_file_name.lpstrCustomFilter = nullptr;
	open_file_name.nMaxCustFilter = 0;
	open_file_name.nFilterIndex = 0;
	open_file_name.lpstrFile = output;
	open_file_name.nMaxFile = count;
	open_file_name.lpstrFileTitle = nullptr;
	open_file_name.nMaxFileTitle = 0;
	open_file_name.lpstrInitialDir = nullptr;
	open_file_name.lpstrTitle = nullptr;
	open_file_name.Flags = OFN_FILEMUSTEXIST;
	open_file_name.nFileOffset = -1;
	open_file_name.nFileExtension = -1;
	open_file_name.lpstrDefExt = nullptr;
	open_file_name.lCustData = -1;
	open_file_name.lpfnHook = nullptr;
	open_file_name.lpTemplateName = nullptr;
	// reserved members are uninitialized ...
	open_file_name.FlagsEx = 0;

	if (GetOpenFileNameW(&open_file_name) == FALSE) {
		// if CommDlgExtendedError() returns zero, the user closed the dialog
		if (CommDlgExtendedError() != 0)
		{
			throw runtime_error("error opening file");
		}
	};

	return output;
}

char hexadecimal_digits[] = "0123456789abcdef";

// Convert a byte to a 2-digit hexadecimal string
void byte_to_string(string& out, u8 byte) {
	out.resize(2);
	out[1] = hexadecimal_digits[(byte & 0b0000'1111) >> 0];
	out[0] = hexadecimal_digits[(byte & 0b1111'0000) >> 4];
}

// Convert an integer to a 8-digit hexadecimal string
void int_to_string8(string& out, unsigned num) {
	out.resize(8);
	for (int i = 0; i < 8; i++)
		out[7 - i] = hexadecimal_digits[(num & (0b1111 << (i * 4))) >> (i * 4)];
}

char to_printable_char(u8 ch) {
	if (0x00 <= ch && ch <= 0x1f
		|| 0x7f <= ch)
		return '.';
	return ch;
}
