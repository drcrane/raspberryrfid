#include <stdio.h>
#include <jpeglib.h>
#include <stdlib.h>
#include <string.h>

#include "imagefn.hpp"

int get_jpeg_size(char *filename, unsigned long int *width, unsigned long int *height)
{
	/* these are standard libjpeg structures for reading(decompression) */
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	/* libjpeg data structure for storing one row, that is, scanline of an image */
	//JSAMPROW row_pointer[1];

	FILE *infile = fopen(filename, "rb");
	//unsigned long location = 0;
	//int i = 0;

	if ( !infile ) {
		return -1;
	}
	/* here we set up the standard libjpeg error handler */
	cinfo.err = jpeg_std_error( &jerr );
	/* setup decompression process and source, then read JPEG header */
	jpeg_create_decompress(&cinfo);
	/* this makes the library read from infile */
	jpeg_stdio_src(&cinfo, infile);
	/* reading the image header which contains image information */
	jpeg_read_header(&cinfo, TRUE);

	*width = cinfo.image_width;
	*height = cinfo.image_height;
	
	fclose(infile);

	printf( "JPEG File Information: \n" );
	printf( "Image width and height: %d pixels and %d pixels.\n", cinfo.image_width, cinfo.image_height );
	printf( "Color components per pixel: %d.\n", cinfo.num_components );
	printf( "Color space: %d.\n", cinfo.jpeg_color_space );
	
	return 1;
}

/*
 * After dumping this nice raw file you should be able to view it with ImageMagick
 * convert -depth 8 -size 3008x2000 bgr:out.raw show:
 * Naturally you will need to change the size and stuff
*/

int decode_jpeg_from_buffer(void ** rawbuffer, unsigned long int * height, unsigned long int * width, unsigned char * jpegbuffer, unsigned long int jpegbuffersz) {
	/* these are standard libjpeg structures for reading(decompression) */
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	/* libjpeg data structure for storing one row, that is, scanline of an image */
	JSAMPROW row_pointer[1];
	unsigned char * raw_image;
	
	unsigned long location = 0;
	unsigned int i = 0;

	/* here we set up the standard libjpeg error handler */
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	jpeg_mem_src(&cinfo, jpegbuffer, jpegbuffersz);
	/* reading the image header which contains image information */
	jpeg_read_header(&cinfo, TRUE);

	*width = cinfo.image_width;
	*height = cinfo.image_height;
	
	//fprintf(stdout, "JPEG File Information: \n" );
	//fprintf(stdout, "Image width and height: %d pixels and %d pixels.\n", cinfo.image_width, cinfo.image_height );
	//fprintf(stdout, "Color components per pixel: %d.\n", cinfo.num_components );
	//fprintf(stdout, "Color space: %d.\n", cinfo.jpeg_color_space );
	
	jpeg_start_decompress( &cinfo );

	raw_image = (unsigned char*)malloc( cinfo.output_width*cinfo.output_height*cinfo.num_components );
	row_pointer[0] = (unsigned char *)malloc( cinfo.output_width*cinfo.num_components );
	while( cinfo.output_scanline < cinfo.image_height ) {
		jpeg_read_scanlines( &cinfo, row_pointer, 1 );
		for( i=0; i < cinfo.image_width*cinfo.num_components;i++)
			raw_image[location++] = row_pointer[0][i];
	}
	
	jpeg_finish_decompress( &cinfo );
	jpeg_destroy_decompress( &cinfo );
	free( row_pointer[0] );
	
	return 0;
}
