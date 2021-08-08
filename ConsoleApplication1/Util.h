#pragma once

#include <string>

#include "ints.h"

std::wstring open_file_dialog();

void byte_to_string(std::string& out, u8 byte);
void int_to_string8(std::string& out, unsigned num);
char to_printable_char(u8 ch);