/*
 * Copyright (C) 2015 NIPE-SYSTEMS
 * Copyright (C) 2015 Hauke Oldsen
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include "include-core.h"
#include "include-openvg.h"
#include "include-freeimage.h"
#include "image-util.h"
#include "log-util.h"
#include "egl-util.h"

static char encoding_table[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/' };
static int mod_table[] = { 0, 2, 1 };

/**
 * Destroys the OpenVG image and frees the image structure.
 *
 * @param image A pointer to an image structure
 */
void image_cleanup(image_t *image)
{
	vgDestroyImage(image->image);
	free(image);
}

/**
 * Loads a bitmap. If necessary, it will be converted to a 32 bpp bitmap.
 * Supported formats: http://freeimage.sourceforge.net/features.html
 * 
 * @param path The path of a bitmap file
 * @return The bitmap
 */
FIBITMAP *image_load_bitmap(const char *path)
{
	//printf("Loading image %i\n");
	
	FIBITMAP *bitmap = FreeImage_Load(FreeImage_GetFileType(path, 0), path, 0);
	if(!bitmap)
	{
		eprintf("Failed to load bitmap %s\n", path);
		
		return NULL;
	}
	
	//FreeImage_FlipVertical(bitmap);
	// printf("w=%i h=%i\n", FreeImage_GetWidth(bitmap), FreeImage_GetHeight(bitmap));
	
	if(FreeImage_GetBPP(bitmap) != 32)
	{
		FIBITMAP *old = bitmap;
		bitmap = FreeImage_ConvertTo32Bits(bitmap);
		FreeImage_Unload(old);
	}
	
	return bitmap;
}

/**
 * Creates an OpenVG image using the given data. Each scanline is assumed to be width * 4
 *
 * @param format Format of data. This must be a 4-byte format (e.g. VG_sRGBA_8888 or VG_sARGB_8888)
 * @param width The width
 * @param height The height
 * @param Pointer to data 
 * @return A pointer to an allocated image structure
 */
image_t *image_create(VGImageFormat format, VGint width, VGint height, const void *data)
{
	image_t *image = malloc(sizeof(image_t));
	image->width = width;
	image->height = height;
	image->image = vgCreateImage(format, image->width, image->height, VG_IMAGE_QUALITY_BETTER);
	vgImageSubData(image->image, data, image->width * 4, format, 0, 0, image->width, image->height);
	
	return image;
}

/**
 * Frees a bitmap. 
 *
 * @param bitmap The bitmap
 */
void image_free_bitmap(FIBITMAP *bitmap)
{
	FreeImage_Unload(bitmap);
}

/**
 * Creates a new image. It is a combination of image_load_bitmap, image_create, image_free_bitmap
 *
 * @param path Path to a bitmap
 * @return The new image
 */
image_t *image_load(const char *path)
{
	FIBITMAP *bitmap = image_load_bitmap(path);
	image_t *img = image_create(VG_sARGB_8888,  FreeImage_GetWidth(bitmap),  FreeImage_GetHeight(bitmap), FreeImage_GetBits(bitmap));
	image_free_bitmap(bitmap);
	return img;
}

/**
 * Encodes data with base64
 *
 * @param start_prefix This string will be used as prefix for the output string
 * @param data The binary data
 * @param input_length Length of data
 * @return A base64-encoded representation of data
 */
static char *image_base64_encode(char *start_prefix, const unsigned char *data, size_t input_length)
{
	char *encoded_data = NULL;
	unsigned int octet_a = 0;
	unsigned int octet_b = 0;
	unsigned int octet_c = 0;
	unsigned int triple = 0;
	int i = 0;
	int j = 0;
	size_t output_length = 0;
	
	if(start_prefix == NULL)
	{
		start_prefix = "";
	}
	
	output_length = strlen(start_prefix) + 4 * ((input_length + 2) / 3 + 1);
	
	encoded_data = malloc(output_length);
	if(encoded_data == NULL)
	{
		return NULL;
	}
	
	for(i = 0; (unsigned int)i < strlen(start_prefix); i++)
	{
		encoded_data[i] = start_prefix[i];
	}
	
	for(i = 0, j = strlen(start_prefix); (unsigned int)i < input_length;)
	{
		octet_a = (unsigned int)i < input_length ? (unsigned char)data[i++] : 0;
		octet_b = (unsigned int)i < input_length ? (unsigned char)data[i++] : 0;
		octet_c = (unsigned int)i < input_length ? (unsigned char)data[i++] : 0;
		
		triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;
		
		encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
		encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
		encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
		encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
	}
	
	for(i = 0; i < mod_table[input_length % 3]; i++)
	{
		encoded_data[output_length - 1 - i - 1] = '=';
	}
	
	encoded_data[output_length - 1] = '\0';
	
	return encoded_data;
}

/**
 * Returns a data-URL containing a representation of src in the format specified by the type
 *
 * @param src Raw image data of whole screen
 * @param type Format (currently supported: image/png and image/jpeg)
 * @param encoder_options Only used for image/jpeg. Specifies the quality of the output image.
 * @return The data-URL (must be freed)
 */
char *image_to_data_url(char *src, const char *type, float encoder_options)
{
	unsigned int x = 0;
	unsigned int y = 0;
	RGBQUAD color = { 0, 0, 0, 0 };
	FIMEMORY *memory_stream = NULL;
	FIBITMAP *image = FreeImage_Allocate(egl_get_width(), egl_get_height(), 32, 0xFF000000, 0x00FF0000, 0x0000FF00);
	char *data_base64 = NULL;
	size_t data_amount = 0;
	FREE_IMAGE_FORMAT save_format = FIF_PNG;
	int save_flags = 0;
	char *save_prefix = "";
	
	if(!image)
	{
		eprintf("Failed to create data url.\n");
		
		return NULL;
	}
	
	for(y = 0; y < (unsigned int) egl_get_height(); y++)
	{
		for(x = 0; x < (unsigned int) egl_get_width(); x++)
		{
			color.rgbRed = src[(y * egl_get_width() + x) * 4 + 3];
			color.rgbGreen = src[(y * egl_get_width() + x) * 4 + 2];
			color.rgbBlue = src[(y * egl_get_width() + x) * 4 + 1];
			color.rgbReserved = src[(y * egl_get_width() + x) * 4 + 0];
			
			FreeImage_SetPixelColor(image, x, y, &color);
		}
	}
	
	if(!strcmp(type, "image/png"))
	{
		save_format = FIF_PNG;
		save_flags = PNG_DEFAULT;
		save_prefix = "data:image/png;base64,";
	}
	else if(!strcmp(type, "image/jpeg"))
	{
		save_format = FIF_JPEG;
		if(encoder_options <= 1 && encoder_options >= 0)
		{
			save_flags = encoder_options * 100;
		}
		else
		{
			save_flags = JPEG_DEFAULT;
		}
		save_prefix = "data:image/jpeg;base64,";
	}
	else
	{
		save_format = FIF_PNG;
		save_flags = PNG_DEFAULT;
		save_prefix = "data:image/png;base64,";
	}
	
	memory_stream = FreeImage_OpenMemory(NULL, 0);
	
	if(!FreeImage_SaveToMemory(save_format, image, memory_stream, save_flags))
	{
		eprintf("Failed to save data url to memory.\n");
		
		FreeImage_Unload(image);
		FreeImage_CloseMemory(memory_stream);
		
		return NULL;
	}
	
	FreeImage_Unload(image);
	
	BYTE *mem_data;
	
	if(!FreeImage_AcquireMemory(memory_stream, &mem_data, &data_amount))
	{
		eprintf("Failed to acquire data url.\n");
		
		FreeImage_CloseMemory(memory_stream);
		
		return NULL;
	}
	
	data_base64 = image_base64_encode(save_prefix, mem_data, data_amount);
	
	FreeImage_CloseMemory(memory_stream);
	
	return data_base64;
}

/**
 * Creates a blob representing src
 *
 * @param src Raw image data of whole screen
 * @param type Format (currently supported: image/png and image/jpeg)
 * @param encoder_options Only used for image/jpeg. Specifies the quality of the output image.
 * @param data_amount Pointer where to write the blob's size to
 * @return Pointer to blob (must be freed)
 */
char *image_to_blob(char *src, const char *type, float encoder_options, size_t *data_amount)
{
	unsigned int x = 0;
	unsigned int y = 0;
	RGBQUAD color = { 0, 0, 0, 0 };
	FIMEMORY *memory_stream = NULL;
	FIBITMAP *image = FreeImage_Allocate(egl_get_width(), egl_get_height(), 32, 0xFF000000, 0x00FF0000, 0x0000FF00);
	char *data_copy = NULL;
	FREE_IMAGE_FORMAT save_format = FIF_PNG;
	int save_flags = 0;
	
	if(/*data == NULL || */!image)
	{
		eprintf("Failed to create blob.\n");
		
		*data_amount = 0;
		
		return NULL;
	}

	for(y = 0; y < (unsigned int)egl_get_height(); y++)
	{
		for(x = 0; x < (unsigned int)egl_get_width(); x++)
		{
			color.rgbRed = src[(y * egl_get_width() + x) * 4 + 3];
			color.rgbGreen = src[(y * egl_get_width() + x) * 4 + 2];
			color.rgbBlue = src[(y * egl_get_width() + x) * 4 + 1];
			color.rgbReserved = src[(y * egl_get_width() + x) * 4 + 0];
			
			FreeImage_SetPixelColor(image, x, y, &color);
		}
	}
	
	if(!strcmp(type, "image/png"))
	{
		save_format = FIF_PNG;
		save_flags = PNG_DEFAULT;
	}
	else if(!strcmp(type, "image/jpeg"))
	{
		save_format = FIF_JPEG;
		if(encoder_options <= 1 && encoder_options >= 0)
		{
			save_flags = encoder_options * 100;
		}
		else
		{
			save_flags = JPEG_DEFAULT;
		}
	}
	else
	{
		save_format = FIF_PNG;
		save_flags = PNG_DEFAULT;
	}
	
	memory_stream = FreeImage_OpenMemory(NULL, 0);
	
	if(!FreeImage_SaveToMemory(save_format, image, memory_stream, save_flags))
	{
		eprintf("Failed to save blob to memory.\n");
		
		FreeImage_Unload(image);
		FreeImage_CloseMemory(memory_stream);
		
		*data_amount = 0;
		
		return NULL;
	}
	
	FreeImage_Unload(image);
	
	BYTE *mem_data;
	
	if(!FreeImage_AcquireMemory(memory_stream, &mem_data, data_amount))
	{
		eprintf("Failed to acquire blob.\n");
		
		FreeImage_CloseMemory(memory_stream);
		
		*data_amount = 0;
		
		return NULL;
	}
	
	data_copy = malloc(*data_amount);
	if(data_copy == NULL)
	{
		eprintf("Failed to acquire blob.\n");
		
		FreeImage_CloseMemory(memory_stream);
		
		*data_amount = 0;
		
		return NULL;
	}
	
	memcpy(data_copy, mem_data, *data_amount);
	
	FreeImage_CloseMemory(memory_stream);
	
	return data_copy;
}
