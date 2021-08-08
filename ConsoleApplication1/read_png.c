/*- pngtopixels, derived from pngtopng
 *
 * COPYRIGHT: Written by John Cunningham Bowler, 2011, 2017.
 * To the extent possible under law, the author has waived all copyright and
 * related or neighboring rights to this work.  This work is published from:
 * United States.
 * ...
 */
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <png.h>

#include "read_png.h"

#if defined(PNG_SIMPLIFIED_READ_SUPPORTED) && \
    defined(PNG_SIMPLIFIED_WRITE_SUPPORTED)

u8* read_png(const char* filename, int* out_width, int* out_height)
{
	u8* result = NULL;

	png_image image;

	/* Only the image structure version number needs to be set. */
	memset(&image, 0, sizeof image);
	image.version = PNG_IMAGE_VERSION;

	if (png_image_begin_read_from_file(&image, filename))
	{
		png_bytep buffer;

		/* Change this to try different formats!  If you set a colormap format
		 * then you must also supply a colormap below.
		 */
		image.format = PNG_FORMAT_RGBA;

		buffer = malloc(PNG_IMAGE_SIZE(image));

		if (buffer != NULL)
		{
			int pitch = PNG_IMAGE_ROW_STRIDE(image) * PNG_IMAGE_PIXEL_COMPONENT_SIZE(image.format);
			//printf("image pitch: %i\n", pitch);

			//memset(buffer, 0x00, PNG_IMAGE_SIZE(image));

			if (pitch == image.width * PNG_IMAGE_PIXEL_CHANNELS(image.format) * PNG_IMAGE_PIXEL_COMPONENT_SIZE(image.format)) {
				//printf("image pixel: %i,%i,%i,%i\n", buffer[0], buffer[1], buffer[2], buffer[3]);
				if (png_image_finish_read(&image, NULL/*background*/, buffer,
					0/*row_stride*/, NULL/*colormap for PNG_FORMAT_FLAG_COLORMAP */))
				{
					//printf("image pixel: %i,%i,%i,%i\n", buffer[0], buffer[1], buffer[2], buffer[3]);
					//int shift = 25 * pitch + 25 * 4;
					//printf("image pixel: %i,%i,%i,%i\n", buffer[shift + 0], buffer[shift + 1], buffer[shift + 2], buffer[shift + 3]);

					*out_width = image.width;
					*out_height = image.height;
					result = buffer;
				}

				else
					fprintf(stderr, "pngtopng: read %s: %s\n", filename,
						image.message);
			}

			else
				fprintf(stderr, "image pitch is not minimal!!!!!\n");
		}

		else
		{
			fprintf(stderr, "pngtopng: out of memory: %lu bytes\n",
				(unsigned long)PNG_IMAGE_SIZE(image));

			/* This is the only place where a 'free' is required; libpng does
			 * the cleanup on error and success, but in this case we couldn't
			 * complete the read because of running out of memory and so libpng
			 * has not got to the point where it can do cleanup.
			 */
			png_image_free(&image);
		}
	}

	else
		/* Failed to read the first argument: */
		fprintf(stderr, "pngtopng: %s: %s\n", filename, image.message);

	return result;
}
#endif /* READ && WRITE */
