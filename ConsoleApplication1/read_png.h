#pragma once

#include "ints.h"

#ifdef __cplusplus
extern "C" {
#endif
	u8* read_png(const char* filename, int* out_width, int* out_height);
#ifdef __cplusplus
}
#endif
